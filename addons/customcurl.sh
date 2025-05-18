#!/bin/bash
######################################################
# written by George Liu (eva2000) centminmod.com
# custom curl yum repo addon installer
# use at own risk as it can break the system
# info at http://nervion.us.es/city-fan/yum-repo/
######################################################
# variables
#############
DT=$(date +"%d%m%y-%H%M%S")
CENTMINLOGDIR='/root/centminlogs'
DIR_TMP='/svr-setup'

# custom curl/libcurl RPM for 7.47 and higher
# enable with CUSTOM_CURLRPM=y
# use at own risk as it can break the system
# info at http://nervion.us.es/city-fan/yum-repo/
CUSTOM_CURLRPM=y
CUSTOM_CURL_EL89=n

FORCE_IPVFOUR='y' # curl/wget commands through script force IPv4
###############################################################
# set locale temporarily to english
# due to some non-english locale issues
export LC_ALL=en_US.UTF-8
export LANG=en_US.UTF-8
export LANGUAGE=en_US.UTF-8
export LC_CTYPE=en_US.UTF-8
# disable systemd pager so it doesn't pipe systemctl output to less
export SYSTEMD_PAGER=''
ARCH_CHECK="$(uname -m)"

shopt -s expand_aliases
for g in "" e f; do
    alias ${g}grep="LC_ALL=C ${g}grep"  # speed-up grep, egrep, fgrep
done

CENTOSVER=$(awk '{ print $3 }' /etc/redhat-release)

if [ "$CENTOSVER" == 'release' ]; then
    CENTOSVER=$(awk '{ print $4 }' /etc/redhat-release | cut -d . -f1,2)
    if [[ "$(cat /etc/redhat-release | awk '{ print $4 }' | cut -d . -f1)" = '7' ]]; then
        CENTOS_SEVEN='7'
    elif [[ "$(cat /etc/redhat-release | awk '{ print $4 }' | cut -d . -f1)" = '8' ]]; then
        CENTOS_EIGHT='8'
    elif [[ "$(cat /etc/redhat-release | awk '{ print $4 }' | cut -d . -f1)" = '9' ]]; then
        CENTOS_NINE='9'
    elif [[ "$(cat /etc/redhat-release | awk '{ print $4 }' | cut -d . -f1)" = '10' ]]; then
        CENTOS_TEN='10'
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

# ensure only el8+ OS versions are being looked at for alma linux, rocky linux
# oracle linux, vzlinux, circle linux, navy linux, euro linux
EL_VERID=$(awk -F '=' '/VERSION_ID/ {print $2}' /etc/os-release | sed -e 's|"||g' | cut -d . -f1)
if [ -f /etc/almalinux-release ] && [[ "$EL_VERID" -eq 8 || "$EL_VERID" -eq 9 || "$EL_VERID" -eq 10 ]]; then
  CENTOSVER=$(awk '{ print $3 }' /etc/almalinux-release | cut -d . -f1,2)
  ALMALINUXVER=$(awk '{ print $3 }' /etc/almalinux-release | cut -d . -f1,2 | sed -e 's|\.|000|g')
  if [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '8' ]]; then
    CENTOS_EIGHT='8'
    ALMALINUX_EIGHT='8'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '9' ]]; then
    CENTOS_NINE='9'
    ALMALINUX_NINE='9'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '10' ]]; then
    CENTOS_TEN='10'
    ALMALINUX_TEN='10'
  fi
elif [ -f /etc/rocky-release ] && [[ "$EL_VERID" -eq 8 || "$EL_VERID" -eq 9 || "$EL_VERID" -eq 10 ]]; then
  CENTOSVER=$(awk '{ print $4 }' /etc/rocky-release | cut -d . -f1,2)
  ROCKYLINUXVER=$(awk '{ print $3 }' /etc/rocky-release | cut -d . -f1,2 | sed -e 's|\.|000|g')
  if [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '8' ]]; then
    CENTOS_EIGHT='8'
    ROCKYLINUX_EIGHT='8'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '9' ]]; then
    CENTOS_NINE='9'
    ROCKYLINUX_NINE='9'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '10' ]]; then
    CENTOS_TEN='10'
    ROCKYLINUX_TEN='10'
  fi
elif [ -f /etc/oracle-release ] && [[ "$EL_VERID" -eq 8 || "$EL_VERID" -eq 9 || "$EL_VERID" -eq 10 ]]; then
  CENTOSVER=$(awk '{ print $5 }' /etc/oracle-release | cut -d . -f1,2)
  if [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '8' ]]; then
    CENTOS_EIGHT='8'
    ORACLELINUX_EIGHT='8'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '9' ]]; then
    CENTOS_NINE='9'
    ORACLELINUX_NINE='9'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '10' ]]; then
    CENTOS_TEN='10'
    ORACLELINUX_TEN='10'
  fi
elif [ -f /etc/vzlinux-release ] && [[ "$EL_VERID" -eq 8 || "$EL_VERID" -eq 9 || "$EL_VERID" -eq 10 ]]; then
  CENTOSVER=$(awk '{ print $4 }' /etc/vzlinux-release | cut -d . -f1,2)
  if [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '8' ]]; then
    CENTOS_EIGHT='8'
    VZLINUX_EIGHT='8'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '9' ]]; then
    CENTOS_NINE='9'
    VZLINUX_NINE='9'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '10' ]]; then
    CENTOS_TEN='10'
    VZLINUX_TEN='10'
  fi
elif [ -f /etc/circle-release ] && [[ "$EL_VERID" -eq 8 || "$EL_VERID" -eq 9 || "$EL_VERID" -eq 10 ]]; then
  CENTOSVER=$(awk '{ print $4 }' /etc/circle-release | cut -d . -f1,2)
  if [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '8' ]]; then
    CENTOS_EIGHT='8'
    CIRCLELINUX_EIGHT='8'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '9' ]]; then
    CENTOS_NINE='9'
    CIRCLELINUX_NINE='9'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '10' ]]; then
    CENTOS_TEN='10'
    CIRCLELINUX_TEN='10'
  fi
elif [ -f /etc/navylinux-release ] && [[ "$EL_VERID" -eq 8 || "$EL_VERID" -eq 9 || "$EL_VERID" -eq 10 ]]; then
  CENTOSVER=$(awk '{ print $5 }' /etc/navylinux-release | cut -d . -f1,2)
  if [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '8' ]]; then
    CENTOS_EIGHT='8'
    NAVYLINUX_EIGHT='8'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '9' ]]; then
    CENTOS_NINE='9'
    NAVYLINUX_NINE='9'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '10' ]]; then
    CENTOS_TEN='10'
    NAVYLINUX_TEN='10'
  fi
elif [ -f /etc/el-release ] && [[ "$EL_VERID" -eq 8 || "$EL_VERID" -eq 9 || "$EL_VERID" -eq 10 ]]; then
  CENTOSVER=$(awk '{ print $3 }' /etc/el-release | cut -d . -f1,2)
  if [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '8' ]]; then
    CENTOS_EIGHT='8'
    EUROLINUX_EIGHT='8'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '9' ]]; then
    CENTOS_NINE='9'
    EUROLINUX_NINE='9'
  elif [[ "$(echo $CENTOSVER | cut -d . -f1)" -eq '10' ]]; then
    CENTOS_TEN='10'
    EUROLINUX_TEN='10'
  fi
fi

if [ -f /etc/centminmod/custom_config.inc ]; then
  source /etc/centminmod/custom_config.inc
fi
if [[ "$FORCE_IPVFOUR" != [yY] ]]; then
  ipv_forceopt=""
  ipv_forceopt_wget=""
  WGETOPT="-cnv --no-dns-cache${ipv_forceopt_wget}"
else
  ipv_forceopt='4'
  ipv_forceopt_wget=' -4'
  WGETOPT="-cnv --no-dns-cache${ipv_forceopt_wget}"
fi

if [ ! -d "$DIR_TMP" ]; then
  mkdir -p "$DIR_TMP"
  chmod 0750 "$DIR_TMP"
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

###########################################
# functions
#############
if [[ "$CENTOS_EIGHT" -eq '8' || "$CENTOS_NINE" -eq '9' || "$CENTOS_TEN" -eq '10' ]] && [[ "$CUSTOM_CURL_EL89" != [yY] ]]; then
  echo "$0 only for CentOS 7"
  echo "aborted..."
  exit 1
fi


curlrpm() {
if [[ "$CUSTOM_CURLRPM" = [yY] ]]; then
  if [ -f "/usr/local/src/centminmod/downloads/curlrpms.zip" ]; then
    /usr/bin/unzip -qo "/usr/local/src/centminmod/downloads/curlrpms.zip" -d "$DIR_TMP"/
  fi
  ###############################################################
  if [[ "$CENTOS_SIX" = '6' && "$(uname -m)" != 'x86_64' ]]; then
  #############################
  # el6 32bit
  curl -${ipv_forceopt}Is --connect-timeout 30 --max-time 30 http://www.city-fan.org/ftp/contrib/yum-repo/city-fan.org-release-2-1.rhel6.noarch.rpm
  CURL_NOARCHRPMCHECK=$?
  if [[ "$CURL_NOARCHRPMCHECK" = '0' ]]; then
    rpm -Uvh http://www.city-fan.org/ftp/contrib/yum-repo/city-fan.org-release-2-1.rhel6.noarch.rpm
  else
    if [ -f "$DIR_TMP/city-fan.org-release-2-1.rhel6.noarch.rpm" ]; then
      rpm -Uvh "$DIR_TMP/city-fan.org-release-2-1.rhel6.noarch.rpm"
    fi
  fi
  sed -i 's|enabled=1|enabled=0|g' /etc/yum.repos.d/city-fan.org.repo
  if [ -f /etc/yum.repos.d/city-fan.org.repo ]; then
    cp -p /etc/yum.repos.d/city-fan.org.repo /etc/yum.repos.d/city-fan.org.OLD
    if [ -n "$(grep ^priority /etc/yum.repos.d/city-fan.org.repo)" ]; then
      #echo priorities already set for city-fan.org.repo
      PRIOREXISTS=1
    else
      echo "setting yum priorities for city-fan.org.repo"
      sed -i 's|^gpgkey=.*|&\npriority=99\nexcludes=libtidy libtidy-devel libidn libidn-devel libgs libgs-devel|' /etc/yum.repos.d/city-fan.org.repo
    fi
  fi # repo file check
  yum versionlock libidn libidn-devel libgs libgs-devel
  yum -y install curl libcurl libcurl-devel libcurl7112 libcurl7155 --enablerepo=city-fan.org --disableplugin=priorities
  echo
  curl -V
  echo
  cecho "recompile PHP via centmin.sh menu option 5 to" $boldyellow
  cecho "complete new curl version setup on your system" $boldyellow
  ###############################################################
  elif [[ "$CENTOS_SIX" = '6' && "$(uname -m)" = 'x86_64' ]]; then
  ###############################################################
  # el6 64bit
  curl -${ipv_forceopt}Is --connect-timeout 30 --max-time 30 http://www.city-fan.org/ftp/contrib/yum-repo/city-fan.org-release-2-1.rhel6.noarch.rpm
  CURL_NOARCHRPMCHECK=$?
  if [[ "$CURL_NOARCHRPMCHECK" = '0' ]]; then
    rpm -Uvh http://www.city-fan.org/ftp/contrib/yum-repo/city-fan.org-release-2-1.rhel6.noarch.rpm
  else
    if [ -f "$DIR_TMP/city-fan.org-release-2-1.rhel6.noarch.rpm" ]; then
      rpm -Uvh "$DIR_TMP/city-fan.org-release-2-1.rhel6.noarch.rpm"
    fi
  fi
  
  sed -i 's|enabled=1|enabled=0|g' /etc/yum.repos.d/city-fan.org.repo
  if [ -f /etc/yum.repos.d/city-fan.org.repo ]; then
    cp -p /etc/yum.repos.d/city-fan.org.repo /etc/yum.repos.d/city-fan.org.OLD
    if [ -n "$(grep ^priority /etc/yum.repos.d/city-fan.org.repo)" ]; then
      #echo priorities already set for city-fan.org.repo
      PRIOREXISTS=1
    else
      echo "setting yum priorities for city-fan.org.repo"
      sed -i 's|^gpgkey=.*|&\npriority=99\nexcludes=libtidy libtidy-devel libidn libidn-devel libgs libgs-devel|' /etc/yum.repos.d/city-fan.org.repo
    fi
  fi # repo file check
  yum versionlock libidn libidn-devel libgs libgs-devel
  yum -y install curl libcurl libcurl-devel libcurl7112 libcurl7155 --enablerepo=city-fan.org --disableplugin=priorities
  echo
  curl -V
  echo
  cecho "recompile PHP via centmin.sh menu option 5 to" $boldyellow
  cecho "complete new curl version setup on your system" $boldyellow
  ###############################################################
  elif [[ "$CENTOS_SEVEN" = '7' && "$(uname -m)" = 'x86_64' ]]; then
  ###############################################################
  # el8 64bit
  cityfan_rpm_name=$(curl -${ipv_forceopt}sL --connect-timeout 30 --max-time 30 https://mirror.city-fan.org/ftp/contrib/yum-repo/| egrep -ow "city-fan.org-release-[0-9.]+\-[0-9.]+\.rhel7\.noarch\.rpm" | grep release | uniq)
  rpm --import https://mirror.city-fan.org/ftp/contrib/GPG-KEYS/RPM-GPG-KEY-city-fan.org-rhel-7
  curl -${ipv_forceopt}Is --connect-timeout 30 --max-time 30 https://mirror.city-fan.org/ftp/contrib/yum-repo/${cityfan_rpm_name}
  CURL_NOARCHRPMCHECK=$?
  if [[ "$CURL_NOARCHRPMCHECK" = '0' ]]; then
    rpm -Uvh https://mirror.city-fan.org/ftp/contrib/yum-repo/${cityfan_rpm_name}
  else
    if [ -f "$DIR_TMP/${cityfan_rpm_name}" ]; then
      rpm -Uvh "$DIR_TMP/${cityfan_rpm_name}"
    fi
  fi
  
  sed -i 's|enabled=1|enabled=0|g' /etc/yum.repos.d/city-fan.org.repo
  if [ -f /etc/yum.repos.d/city-fan.org.repo ]; then
    cp -p /etc/yum.repos.d/city-fan.org.repo /etc/yum.repos.d/city-fan.org.OLD
    if [ -n "$(grep ^priority /etc/yum.repos.d/city-fan.org.repo)" ]; then
      #echo priorities already set for city-fan.org.repo
      PRIOREXISTS=1
    else
      echo "setting yum priorities for city-fan.org.repo"
      sed -i 's|^gpgkey=.*|&\npriority=99\nexcludes=libtidy libtidy-devel libidn libidn-devel libgs libgs-devel|' /etc/yum.repos.d/city-fan.org.repo
    fi
  fi # repo file check

  yum versionlock libidn libidn-devel libgs libgs-devel
  yum -y install curl libcurl libcurl-devel libcurl7112 libcurl7155 --enablerepo=city-fan.org --disableplugin=priorities
  echo
  curl -V
  echo
  cecho "recompile PHP via centmin.sh menu option 5 to" $boldyellow
  cecho "complete new curl version setup on your system" $boldyellow
  ###############################################################
  elif [[ "$CENTOS_EIGHT" = '8' && "$(uname -m)" = 'x86_64' ]]; then
  ###############################################################
  # el8 64bit
  cityfan_rpm_name=$(curl -${ipv_forceopt}sL --connect-timeout 30 --max-time 30 https://mirror.city-fan.org/ftp/contrib/yum-repo/| egrep -ow "city-fan.org-release-[0-9.]+\-[0-9.]+\.rhel${CENTOS_EIGHT}\.noarch\.rpm" | grep release | uniq)
  rpm --import https://mirror.city-fan.org/ftp/contrib/GPG-KEYS/RPM-GPG-KEY-city-fan.org-rhel-${CENTOS_EIGHT}
  curl -${ipv_forceopt}Is --connect-timeout 30 --max-time 30 https://mirror.city-fan.org/ftp/contrib/yum-repo/${cityfan_rpm_name}
  CURL_NOARCHRPMCHECK=$?
  if [[ "$CURL_NOARCHRPMCHECK" = '0' ]]; then
    rpm -Uvh https://mirror.city-fan.org/ftp/contrib/yum-repo/${cityfan_rpm_name}
  else
    if [ -f "$DIR_TMP/${cityfan_rpm_name}" ]; then
      rpm -Uvh "$DIR_TMP/${cityfan_rpm_name}"
    fi
  fi
  
  sed -i 's|enabled=1|enabled=0|g' /etc/yum.repos.d/city-fan.org.repo
  if [ -f /etc/yum.repos.d/city-fan.org.repo ]; then
    cp -p /etc/yum.repos.d/city-fan.org.repo /etc/yum.repos.d/city-fan.org.OLD
    if [ -n "$(grep ^priority /etc/yum.repos.d/city-fan.org.repo)" ]; then
      #echo priorities already set for city-fan.org.repo
      PRIOREXISTS=1
    else
      echo "setting yum priorities for city-fan.org.repo"
      sed -i 's|^gpgkey=.*|&\npriority=99\n#excludes=libtidy libtidy-devel libidn libidn-devel libgs libgs-devel|' /etc/yum.repos.d/city-fan.org.repo
    fi
  fi # repo file check
  
  yum versionlock libidn libidn-devel libgs libgs-devel
  yum -y install curl libcurl libcurl-devel --enablerepo=city-fan.org --disableplugin=priorities
  echo
  curl -V
  echo
  cecho "recompile PHP via centmin.sh menu option 5 to" $boldyellow
  cecho "complete new curl version setup on your system" $boldyellow
  ###############################################################
  elif [[ "$CENTOS_NINE" = '9' && "$(uname -m)" = 'x86_64' ]]; then
  ###############################################################
  # el9 64bit
  cityfan_rpm_name=$(curl -${ipv_forceopt}sL --connect-timeout 30 --max-time 30 https://mirror.city-fan.org/ftp/contrib/yum-repo/| egrep -ow "city-fan.org-release-[0-9.]+\-[0-9.]+\.rhel${CENTOS_EIGHT}\.noarch\.rpm" | grep release | uniq)
  rpm --import https://mirror.city-fan.org/ftp/contrib/GPG-KEYS/RPM-GPG-KEY-city-fan.org-rhel-${CENTOS_EIGHT}
  curl -${ipv_forceopt}Is --connect-timeout 30 --max-time 30 https://mirror.city-fan.org/ftp/contrib/yum-repo/${cityfan_rpm_name}
  CURL_NOARCHRPMCHECK=$?
  if [[ "$CURL_NOARCHRPMCHECK" = '0' ]]; then
    rpm -Uvh https://mirror.city-fan.org/ftp/contrib/yum-repo/${cityfan_rpm_name}
  else
    if [ -f "$DIR_TMP/${cityfan_rpm_name}" ]; then
      rpm -Uvh "$DIR_TMP/${cityfan_rpm_name}"
    fi
  fi
  
  sed -i 's|enabled=1|enabled=0|g' /etc/yum.repos.d/city-fan.org.repo
  if [ -f /etc/yum.repos.d/city-fan.org.repo ]; then
    cp -p /etc/yum.repos.d/city-fan.org.repo /etc/yum.repos.d/city-fan.org.OLD
    if [ -n "$(grep ^priority /etc/yum.repos.d/city-fan.org.repo)" ]; then
      #echo priorities already set for city-fan.org.repo
      PRIOREXISTS=1
    else
      echo "setting yum priorities for city-fan.org.repo"
      sed -i 's|^gpgkey=.*|&\npriority=99\n#excludes=libtidy libtidy-devel libidn libidn-devel libgs libgs-devel|' /etc/yum.repos.d/city-fan.org.repo
    fi
  fi # repo file check
  
  yum versionlock libidn libidn-devel libgs libgs-devel
  yum -y install curl libcurl libcurl-devel --enablerepo=city-fan.org --disableplugin=priorities
  echo
  curl -V
  echo
  cecho "recompile PHP via centmin.sh menu option 5 to" $boldyellow
  cecho "complete new curl version setup on your system" $boldyellow
  fi
  ###############################################################
fi # CUSTOM_CURLRPM=y
}
##############################################################
starttime=$(TZ=UTC date +%s.%N)
{
curlrpm

echo
cecho "custom curl RPMs installed..." $boldyellow
cecho "you can now use yum update to update curl" $boldyellow
echo
echo " yum update --enablerepo=city-fan.org --disableplugin=priorities"
echo
} 2>&1 | tee "${CENTMINLOGDIR}/centminmod_customcurl_rpms_${DT}.log"

endtime=$(TZ=UTC date +%s.%N)

INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
echo "" >> "${CENTMINLOGDIR}/centminmod_customcurl_rpms_${DT}.log"
echo "Total Custom Curl RPMs Install Time: $INSTALLTIME seconds" >> "${CENTMINLOGDIR}/centminmod_customcurl_rpms_${DT}.log"