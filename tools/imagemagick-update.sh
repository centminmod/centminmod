#!/bin/bash
######################################################
# standalone imagemagick + imagick updater
######################################################
IMAGICKPHP_VER='3.4.3'   # PHP extension for imagick
PHP_INSTALL='y'
PHPIMAGICK='y'
REMIREPO_DISABLE='n'

DT=$(date +"%d%m%y-%H%M%S")
DIR_TMP='/svr-setup'
CONFIGSCANBASE='/etc/centminmod'
CENTMINLOGDIR='/root/centminlogs'
SCRIPT_DIR=$(readlink -f $(dirname ${BASH_SOURCE[0]}))
SCRIPT_SOURCEBASE=$(readlink -f $(dirname ${BASH_SOURCE[0]}))
# account for tools directory placement of tools/nginxupdate.sh
SCRIPT_DIR=$(readlink -f $(dirname ${SCRIPT_DIR}))
######################################################
MACHINE_TYPE=$(uname -m) # Used to detect if OS is 64bit or not.

# set locale temporarily to english
# due to some non-english locale issues
export LC_ALL=en_US.UTF-8
export LANG=en_US.UTF-8
export LANGUAGE=en_US.UTF-8
export LC_CTYPE=en_US.UTF-8

shopt -s expand_aliases
for g in "" e f; do
    alias ${g}grep="LC_ALL=C ${g}grep"  # speed-up grep, egrep, fgrep
done

# source "inc/memcheck.inc"
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

# Check for Redhat Enterprise Linux 7.x
if [ "$CENTOSVER" == 'Enterprise' ]; then
    CENTOSVER=$(awk '{ print $7 }' /etc/redhat-release)
    if [[ "$(awk '{ print $1,$2 }' /etc/redhat-release)" = 'Red Hat' && "$(awk '{ print $7 }' /etc/redhat-release | cut -d . -f1)" = '7' ]]; then
        CENTOS_SEVEN='7'
        REDHAT_SEVEN='y'
    fi
fi

if [[ -f /etc/system-release && "$(awk '{print $1,$2,$3}' /etc/system-release)" = 'Amazon Linux AMI' ]]; then
    CENTOS_SIX='6'
fi

if [ ! -d "$CENTMINLOGDIR" ]; then
    mkdir -p "$CENTMINLOGDIR"
fi
######################################################
# Setup Colours
black='\E[30;40m'
red='\E[31;40m'
green='\E[32;40m'
yellow='\E[33;40m'
blue='\E[34;40m'
magenta='\E[35;40m'
cyan='\E[36;40m'
white='\E[37;40m'

boldblack='\E[1;30;40m'
boldred='\E[1;31;40m'
boldgreen='\E[1;32;40m'
boldyellow='\E[1;33;40m'
boldblue='\E[1;34;40m'
boldmagenta='\E[1;35;40m'
boldcyan='\E[1;36;40m'
boldwhite='\E[1;37;40m'

Reset="tput sgr0"      #  Reset text attributes to normal
                       #+ without clearing screen.

cecho ()                     # Coloured-echo.
                             # Argument $1 = message
                             # Argument $2 = color
{
message=$1
color=$2
echo -e "$color$message" ; $Reset
return
}
######################################################
source "${SCRIPT_DIR}/inc/yumskip.inc"
source "${SCRIPT_DIR}/inc/downloads_centosfive.inc"
source "${SCRIPT_DIR}/inc/downloads_centossix.inc"
source "${SCRIPT_DIR}/inc/downloads_centosseven.inc"
source "${SCRIPT_DIR}/inc/downloadlinks.inc"
source "${SCRIPT_DIR}/inc/downloads.inc"
source "${SCRIPT_DIR}/inc/yumpriorities.inc"
source "${SCRIPT_DIR}/inc/yuminstall.inc"

if [ -f /etc/centminmod/custom_config.inc ]; then
  source /etc/centminmod/custom_config.inc
fi
if [[ "$FORCE_IPVFOUR" != [yY] ]]; then
  ipv_forceopt=""
else
  ipv_forceopt='4'
fi

if [ -f /proc/user_beancounters ]; then
    # CPUS='1'
    # MAKETHREADS=" -j$CPUS"
    # speed up make
    CPUS=$(grep -c "processor" /proc/cpuinfo)
    if [[ "$CPUS" -gt '8' ]]; then
        if [[ "$(grep -o 'AMD EPYC 7601' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7601' ]]; then
            # 7601 at 12 cpu cores has 3.20hz clock frequency https://en.wikichip.org/wiki/amd/epyc/7601
            # while greater than 12 cpu cores downclocks to 2.70Ghz
            CPUS=12
        elif [[ "$(grep -o 'AMD EPYC 7551' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7551' ]]; then
            # 7551P at 12 cpu cores has 3.0Ghz clock frequency https://en.wikichip.org/wiki/amd/epyc/7551p
            # while greater than 12 cpu cores downclocks to 2.55Ghz
            CPUS=12
        elif [[ "$(grep -o 'AMD EPYC 7401' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7401' ]]; then
            # 7401P at 12 cpu cores has 3.0Ghz clock frequency https://en.wikichip.org/wiki/amd/epyc/7401p
            # while greater than 12 cpu cores downclocks to 2.8Ghz
            CPUS=12
        elif [[ "$(grep -o 'AMD EPYC 7371' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7371' ]]; then
            # 7371 at 8 cpu cores has 3.8Ghz clock frequency https://en.wikichip.org/wiki/amd/epyc/7371
            # while greater than 8 cpu cores downclocks to 3.6Ghz
            CPUS=8
        else
            CPUS=$(echo $(($CPUS+2)))
        fi
    else
        CPUS=$(echo $(($CPUS+1)))
    fi
    MAKETHREADS=" -j$CPUS"
else
    # speed up make
    CPUS=$(grep -c "processor" /proc/cpuinfo)
    if [[ "$CPUS" -gt '8' ]]; then
        if [[ "$(grep -o 'AMD EPYC 7601' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7601' ]]; then
            # 7601 at 12 cpu cores has 3.20hz clock frequency https://en.wikichip.org/wiki/amd/epyc/7601
            # while greater than 12 cpu cores downclocks to 2.70Ghz
            CPUS=12
        elif [[ "$(grep -o 'AMD EPYC 7551' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7551' ]]; then
            # 7551P at 12 cpu cores has 3.0Ghz clock frequency https://en.wikichip.org/wiki/amd/epyc/7551p
            # while greater than 12 cpu cores downclocks to 2.55Ghz
            CPUS=12
        elif [[ "$(grep -o 'AMD EPYC 7401' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7401' ]]; then
            # 7401P at 12 cpu cores has 3.0Ghz clock frequency https://en.wikichip.org/wiki/amd/epyc/7401p
            # while greater than 12 cpu cores downclocks to 2.8Ghz
            CPUS=12
        elif [[ "$(grep -o 'AMD EPYC 7371' /proc/cpuinfo | sort -u)" = 'AMD EPYC 7371' ]]; then
            # 7371 at 8 cpu cores has 3.8Ghz clock frequency https://en.wikichip.org/wiki/amd/epyc/7371
            # while greater than 8 cpu cores downclocks to 3.6Ghz
            CPUS=8
        else
            CPUS=$(echo $(($CPUS+4)))
        fi
    elif [[ "$CPUS" -eq '8' ]]; then
        CPUS=$(echo $(($CPUS+2)))
    else
        CPUS=$(echo $(($CPUS+1)))
    fi
    MAKETHREADS=" -j$CPUS"
fi

cmservice() {
        servicename=$1
        action=$2
        if [[ "$CENTOS_SEVEN" != '7' || "${servicename}" = 'php-fpm' || "${servicename}" = 'nginx' || "${servicename}" = 'memcached' || "${servicename}" = 'nsd' || "${servicename}" = 'csf' || "${servicename}" = 'lfd' ]]; then
        echo "service ${servicename} $action"
        if [[ "$CMSDEBUG" = [nN] ]]; then
                service "${servicename}" "$action"
        fi
        else
        echo "systemctl $action ${servicename}.service"
        if [[ "$CMSDEBUG" = [nN] ]]; then
                systemctl "$action" "${servicename}.service"
        fi
        fi
}

cmchkconfig() {
        servicename=$1
        status=$2
        if [[ "$CENTOS_SEVEN" != '7' || "${servicename}" = 'php-fpm' || "${servicename}" = 'nginx' || "${servicename}" = 'memcached' || "${servicename}" = 'nsd' || "${servicename}" = 'csf' || "${servicename}" = 'lfd' ]]; then
        echo "chkconfig ${servicename} $status"
        if [[ "$CMSDEBUG" = [nN] ]]; then
                chkconfig "${servicename}" "$status"
        fi
        else
                if [ "$status" = 'on' ]; then
                        status=enable
                fi
                if [ "$status" = 'off' ]; then
                        status=disable
                fi
        echo "systemctl $status ${servicename}.service"
        if [[ "$CMSDEBUG" = [nN] ]]; then
                systemctl "$status" "${servicename}.service"
        fi
        fi
}

imagick_fixes() {
    if [[ -f /etc/ImageMagick/policy.xml || -f /etc/ImageMagick6/ImageMagick-6/policy.xml ]]; then
        if [ -f "${SCRIPT_DIR}/tools/imagemagick-fix.sh" ]; then
            "${SCRIPT_DIR}/tools/imagemagick-fix.sh" >/dev/null 2>&1
        fi
    fi
}

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
                    yum clean all >/dev/null 2>&1
                    yum -y update ImageMagick6 ImageMagick6-devel ImageMagick6-c++ ImageMagick6-c++-devel --enablerepo=remi --disableplugin=priorities
                fi
            else
                if [[ "$CENTOS_SIX" = '6' ]]; then
                    # yum -y install libwebp libwebp-devel --skip-broken
                    yum clean all >/dev/null 2>&1
                    yum -y install ImageMagick6 ImageMagick6-devel ImageMagick6-c++ ImageMagick6-c++-devel --enablerepo=remi --disableplugin=priorities --skip-broken
                else
                    yum clean all >/dev/null 2>&1
                    yum -y install ImageMagick6 ImageMagick6-devel ImageMagick6-c++ ImageMagick6-c++-devel --enablerepo=remi --disableplugin=priorities
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
                yum -y install lcms2-devel libwebp libwebp-devel OpenEXR-devel ilmbase-devel libGLU-devel libGL-devel mesa-libGL mesa-libGL-devel libXxf86vm libXxf86vm-devel --enablerepo=remi
                yum -y remove ImageMagick
          
                if [[ "$CENTOS_SIX" = '6' ]]; then
                    # yum -y install libwebp libwebp-devel --skip-broken
                    yum -y install ImageMagick6 ImageMagick6-devel ImageMagick6-c++ ImageMagick6-c++-devel --enablerepo=remi --disableplugin=priorities --skip-broken
                else
                    yum -y install ImageMagick6 ImageMagick6-devel ImageMagick6-c++ ImageMagick6-c++-devel --enablerepo=remi --disableplugin=priorities
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
                yum -y install lcms2-devel libwebp libwebp-devel OpenEXR-devel ilmbase-devel libGLU-devel libGL-devel mesa-libGL mesa-libGL-devel libXxf86vm libXxf86vm-devel --enablerepo=remi
                if [[ "$CENTOS_SIX" = '6' ]]; then
                    # yum -y install libwebp libwebp-devel --skip-broken
                    yum -y install ImageMagick6 ImageMagick6-devel ImageMagick6-c++ ImageMagick6-c++-devel --enablerepo=remi --disableplugin=priorities --skip-broken
                else
                    yum -y install ImageMagick6 ImageMagick6-devel ImageMagick6-c++ ImageMagick6-c++-devel --enablerepo=remi --disableplugin=priorities
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

php -v | awk -F " " '{print $2}' | head -n1 | cut -d . -f1,2 | egrep -w '7.0||7.1|7.2|7.3'
PHPSEVEN_CHECKVER=$?
echo $PHPSEVEN_CHECKVER

if [[ "$PHPMUVER" > 7 || "$PHPSEVEN_CHECKVER" = '0' ]] && [[ "$(echo $IMAGICKPHP_VER | cut -d . -f1,2 | sed -e 's|\.||')" -le '33' ]]; then
    IMAGICKGITLINK='https://github.com/mkoppanen/imagick'
    # fallback mirror if official github is down, use gitlab mirror
    curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 $IMAGICKGITLINK | grep 'HTTP\/' | grep '200' >/dev/null 2>&1
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
        time git clone -b phpseven "$IMAGICKGITLINK" imagick-php7
    else
        rm -rf imagick-php7
        time git clone -b phpseven "$IMAGICKGITLINK" imagick-php7
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

case "$1" in
    update )
    {
    imagickinstall
    } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_tools-imagick-updater.log"
        ;;
    * )
    echo
    echo "$0 update"
        ;;
esac