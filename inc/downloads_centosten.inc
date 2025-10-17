###############################################
remitenyum() {

if [[ "$REMIREPO_DISABLE" = [nN] ]]; then
  cd $DIR_TMP
  if [ ! "$(yum repolist remi | grep -o remi)" ]; then
    echo "Configuring Remi EL10 repo"
    rpm --import https://rpms.remirepo.net/RPM-GPG-KEY-remi2018
    rpm --import https://rpms.remirepo.net/RPM-GPG-KEY-remi2019
    rpm --import https://rpms.remirepo.net/RPM-GPG-KEY-remi2020
    rpm --import https://rpms.remirepo.net/RPM-GPG-KEY-remi2021
    rpm --import https://rpms.remirepo.net/RPM-GPG-KEY-remi2022
    rpm --import https://rpms.remirepo.net/RPM-GPG-KEY-remi2023
    rpm --import https://rpms.remirepo.net/RPM-GPG-KEY-remi2024
    echo
    echo "rpm -q gpg-pubkey --qf \"%{summary} ->%{version}-%{release}\n\""
    rpm -q gpg-pubkey --qf "%{summary} ->%{version}-%{release}\n"
    echo
    yum -y install https://rpms.remirepo.net/enterprise/remi-release-10.rpm
    ERR=$?
    CCREMICHECK="$ERR"
    if [ "$CCREMICHECK" == '0' ]; then
      # yumpriorremi_ten
      echo
      grep repo_gpgcheck /etc/yum.repos.d/remi*.repo
      echo
      echo "disable repo_gpgcheck for REMI"
      sed -i 's|repo_gpgcheck=1|repo_gpgcheck=0|g' /etc/yum.repos.d/remi*.repo
      yum clean all
      yum makecache
      echo
      grep repo_gpgcheck /etc/yum.repos.d/remi*.repo
      echo
      yum -y module disable composer
      yum -y module enable redis:remi-7.2
      # el9 missing libc-client& uw-imap-develthat remi-safe/remi repo have
      yum -y install libc-client uw-imap-devel --enablerepo=remi,remi-safe
    else
      cecho "Error: problem with Remi repo install." $boldgreen
    fi
  fi
fi # REMIREPO_DISABLE

}

###############################################
yumcentostendownload() {

if [[ "$CENTOS_TEN" = '10' ]]; then
{
  if [[ "$ALMALINUX_NINE" = '9' ]]; then
    echo "*************************************************"
    cecho "* Alma Linux 10.x detected installing EPEL Repo" $boldgreen
    echo "*************************************************"
  elif [[ "$ROCKYLINUX_NINE" = '9' ]]; then
    echo "*************************************************"
    cecho "* Rocky Linux 10.x detected installing EPEL Repo" $boldgreen
    echo "*************************************************"
  elif [[ "$CENTOS_TEN" = '10' ]]; then
    echo "*************************************************"
    cecho "* CentOS 10.x detected installing EPEL Repo" $boldgreen
    echo "*************************************************"
  fi

  if [ ${MACHINE_TYPE} == 'x86_64' ]; then
    ARCH='x86_64'
    RPMFARCH='x86_64'
  else
    ARCH='i686'
    RPMFARCH='i386'
  fi
  cd $DIR_TMP

  if [ -f /etc/yum.repos.d/epel.repo ]; then
    yumpriorepel_ten
  elif [ ! -f /etc/yum.repos.d/epel.repo ]; then
    yum -y install epel-release
    ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
      cecho "Error: EPEL repo install failed." $boldgreen
      FAILEPEL='y'
      checklogdetails
    elif [[ "$ERROR" = '0' ]]; then
      yumpriorepel_ten
      echo
    fi
  fi

  cd $DIR_TMP
  
  if [ ! "$(yum repolist percona | grep -o percona)" ]; then
    echo "Configuring Percona EL10 repo"
    yum -y install https://repo.percona.com/yum/percona-release-latest.noarch.rpm
    ERR=$?
    CCPERCONACHECK="$ERR"
      if [ "$CCPERCONACHECK" == '0' ]; then
        yumpriorpercona_ten
        percona-release show
        percona-release disable all
        percona-release enable tools
        echo
      else
        cecho "Error: problem with Percona repo install." $boldgreen
      fi  
  fi
  
  if [[ "$(uname -m)" != 'aarch64' ]]; then
    remitenyum
  fi

  # CentOS 9 repo mirrors aren't fully updated so need to specify actual kernel-headers version
  PACKAGEEXTRA='perl-FindBin oniguruma5php oniguruma5php-devel figlet moreutils nghttp2 libnghttp2 libnghttp2-devel jq clang clang-devel jemalloc jemalloc-devel pngquant optipng jpegoptim pwgen pigz pbzip2 xz pxz lz4 libc-client libc-client-devel uw-imap-devel libtidy libtidy-devel libJudy axel bash-completion mlocate re2c libmcrypt libmcrypt-devel kernel-headers kernel-devel cmake'

  declare -a arrays
  arrays=(${PACKAGEEXTRA})

# if curl one liner is used and detected should skip this yum install routine
# since it's already done by curl installer script
if [ ! -f /usr/local/src/curlinstall_yum.txt ]; then

#########################################################
# EL10 BATCH OPTIMIZATION: Group 3 - Utility and Compression Tools
# Reduces ~38 individual DNF calls to 3 batch operations
# Estimated time savings: 2-3 minutes
# Packages split by repository requirements
#########################################################
if [[ "$CENTOS_TEN" -eq '10' ]] && [[ "$CENTOS_TEN_BATCH_INSTALL" = [yY] ]]; then
  echo "*************************************************"
  echo "* Installing EL10 utility and compression tools"
  echo "* Batch optimization: 38 packages in 3 transactions"
  echo "*************************************************"

  # Batch Group 3A: Standard packages (31 packages)
  BATCH_GROUP3A_START=$(date +%s)
  echo "----------------------------------------------------------------------------------"
  cecho "Batch Group 3A: Standard utility packages (31 packages)" $boldyellow
  echo "----------------------------------------------------------------------------------"
  ${YUMDNFBIN}${CACHESKIP} -q -y install \
    perl-FindBin figlet moreutils nghttp2 libnghttp2 libnghttp2-devel \
    jq clang clang-devel jemalloc jemalloc-devel pngquant optipng \
    jpegoptim pwgen pigz pbzip2 xz pxz lz4 libtidy libtidy-devel \
    libJudy axel bash-completion mlocate re2c kernel-headers \
    kernel-devel cmake --skip-broken

  BATCH_GROUP3A_EXIT=$?
  BATCH_GROUP3A_END=$(date +%s)
  BATCH_GROUP3A_DURATION=$((BATCH_GROUP3A_END - BATCH_GROUP3A_START))

  if [[ "$BATCH_GROUP3A_EXIT" -eq 0 ]]; then
    echo "----------------------------------------------------------------------------------"
    echo "* Batch Group 3A completed successfully (${BATCH_GROUP3A_DURATION}s)"
    echo "----------------------------------------------------------------------------------"
    GROUP3A_INSTALLED='y'
  else
    echo "----------------------------------------------------------------------------------"
    echo "* Warning: Batch Group 3A encountered issues - will retry individually"
    echo "----------------------------------------------------------------------------------"
    GROUP3A_INSTALLED='n'
  fi

  # Batch Group 3B: REMI oniguruma packages (2 packages)
  BATCH_GROUP3B_START=$(date +%s)
  echo "----------------------------------------------------------------------------------"
  cecho "Batch Group 3B: REMI oniguruma packages (2 packages)" $boldyellow
  echo "----------------------------------------------------------------------------------"
  ${YUMDNFBIN}${CACHESKIP} -q -y install \
    oniguruma5php oniguruma5php-devel \
    --enablerepo=remi --skip-broken

  BATCH_GROUP3B_EXIT=$?
  BATCH_GROUP3B_END=$(date +%s)
  BATCH_GROUP3B_DURATION=$((BATCH_GROUP3B_END - BATCH_GROUP3B_START))

  if [[ "$BATCH_GROUP3B_EXIT" -eq 0 ]]; then
    echo "----------------------------------------------------------------------------------"
    echo "* Batch Group 3B completed successfully (${BATCH_GROUP3B_DURATION}s)"
    echo "----------------------------------------------------------------------------------"
    GROUP3B_INSTALLED='y'
  else
    echo "----------------------------------------------------------------------------------"
    echo "* Warning: Batch Group 3B encountered issues - will retry individually"
    echo "----------------------------------------------------------------------------------"
    GROUP3B_INSTALLED='n'
  fi

  # Batch Group 3C: REMI IMAP and libmcrypt packages (5 packages)
  BATCH_GROUP3C_START=$(date +%s)
  echo "----------------------------------------------------------------------------------"
  cecho "Batch Group 3C: REMI IMAP and libmcrypt packages (5 packages)" $boldyellow
  echo "----------------------------------------------------------------------------------"
  ${YUMDNFBIN}${CACHESKIP} -q -y install \
    libc-client libc-client-devel uw-imap-devel \
    libmcrypt libmcrypt-devel \
    --enablerepo=remi,remi-safe --skip-broken

  BATCH_GROUP3C_EXIT=$?
  BATCH_GROUP3C_END=$(date +%s)
  BATCH_GROUP3C_DURATION=$((BATCH_GROUP3C_END - BATCH_GROUP3C_START))

  if [[ "$BATCH_GROUP3C_EXIT" -eq 0 ]]; then
    echo "----------------------------------------------------------------------------------"
    echo "* Batch Group 3C completed successfully (${BATCH_GROUP3C_DURATION}s)"
    echo "----------------------------------------------------------------------------------"
    GROUP3C_INSTALLED='y'
  else
    echo "----------------------------------------------------------------------------------"
    echo "* Warning: Batch Group 3C encountered issues - will retry individually"
    echo "----------------------------------------------------------------------------------"
    GROUP3C_INSTALLED='n'
  fi

  # Summary
  TOTAL_GROUP3_DURATION=$((BATCH_GROUP3A_DURATION + BATCH_GROUP3B_DURATION + BATCH_GROUP3C_DURATION))
  echo "*************************************************"
  echo "* Group 3 batch installation completed"
  echo "* Total time: ${TOTAL_GROUP3_DURATION} seconds"
  echo "*************************************************"
  PACKAGEEXTRA_BATCH_INSTALLED='y'
fi

# Original loop - now conditional (preserves EL7/8/9 + EL10 fallback)
if [[ "$PACKAGEEXTRA_BATCH_INSTALLED" != 'y' ]]; then

for pkgs in ${arrays[@]}
do
echo Processing package: $pkgs
YUMINSTALLRPMCHECK=$(rpm -ql $pkgs >/dev/null 2>&1; echo $?)
        if [[ "$YUMINSTALLRPMCHECK" = '0' ]]; then
echo "----------------------------------------------------------------------------------"
        echo "$pkgs already installed"
echo "----------------------------------------------------------------------------------"
        else
echo "----------------------------------------------------------------------------------"
cecho "Installing $pkgs" $boldyellow
echo "----------------------------------------------------------------------------------"
      if [[ "$pkgs" = 'libzip' || "$pkgs" = 'libzip-devel' ]]; then
        yum -y module enable php:remi-7.4
        ${YUMDNFBIN}${CACHESKIP} -q -y install $pkgs
        yum -y module disable php:remi-7.4
      elif [[ "$pkgs" = 'oniguruma5php' || "$pkgs" = 'oniguruma5php-devel' ]]; then
        ${YUMDNFBIN}${CACHESKIP} -q -y install $pkgs --enablerepo=remi --skip-broken
      elif [[ "$pkgs" = 'libc-client' || "$pkgs" = 'libc-client-devel' ]]; then
        ${YUMDNFBIN}${CACHESKIP} -q -y install $pkgs --enablerepo=remi,remi-safe --skip-broken
      elif [[ "$pkgs" = 'uw-imap-devel' ]]; then
        ${YUMDNFBIN}${CACHESKIP} -q -y install $pkgs --enablerepo=remi,remi-safe --skip-broken
      elif [[ "$pkgs" = 'libmcrypt' || "$pkgs" = 'libmcrypt-devel' ]]; then
        ${YUMDNFBIN}${CACHESKIP} -q -y install $pkgs --enablerepo=remi,remi-safe --skip-broken
      else
        ${YUMDNFBIN}${CACHESKIP} -q -y install $pkgs --skip-broken
      fi
        # ${YUMDNFBIN}${CACHESKIP} -q list $pkgs
echo "----------------------------------------------------------------------------------"
        fi
done

fi # PACKAGEEXTRA_BATCH_INSTALLED conditional - closes batch optimization fallback

fi # /usr/local/src/curlinstall_yum.txt

} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_yuminstall_centos10.log

fi # CENTOSVER = 8

}
