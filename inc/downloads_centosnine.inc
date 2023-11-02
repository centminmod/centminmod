###############################################
reminineyum() {

if [[ "$REMIREPO_DISABLE" = [nN] ]]; then
  cd $DIR_TMP
  if [ ! "$(yum repolist remi | grep -o remi)" ]; then
    echo "Configuring Remi EL9 repo"
    rpm --import https://rpms.remirepo.net/RPM-GPG-KEY-remi2018
    rpm --import https://rpms.remirepo.net/RPM-GPG-KEY-remi2019
    rpm --import https://rpms.remirepo.net/RPM-GPG-KEY-remi2020
    rpm --import https://rpms.remirepo.net/RPM-GPG-KEY-remi2021
    rpm --import https://rpms.remirepo.net/RPM-GPG-KEY-remi2022
    rpm --import https://rpms.remirepo.net/RPM-GPG-KEY-remi2023
    echo
    echo "rpm -q gpg-pubkey --qf \"%{summary} ->%{version}-%{release}\n\""
    rpm -q gpg-pubkey --qf "%{summary} ->%{version}-%{release}\n"
    echo
    yum -y install https://rpms.remirepo.net/enterprise/remi-release-9.rpm
    ERR=$?
    CCREMICHECK="$ERR"
    if [ "$CCREMICHECK" == '0' ]; then
      # yumpriorremi_nine
      echo
      # set remi redis 6.2 default
      yum clean all
      yum -y module disable composer
      yum -y module enable redis:remi-6.2
      # el9 missing libc-client& uw-imap-develthat remi-safe/remi repo have
      yum -y install libc-client uw-imap-devel --enablerepo=remi,remi-safe
    else
      cecho "Error: problem with Remi repo install." $boldgreen
    fi
  fi
fi # REMIREPO_DISABLE

}

###############################################
yumcentosninedownload() {

if [[ "$CENTOS_NINE" = '9' ]]; then
{
  if [[ "$ALMALINUX_NINE" = '9' ]]; then
    echo "*************************************************"
    cecho "* Alma Linux 9.x detected installing EPEL Repo" $boldgreen
    echo "*************************************************"
  elif [[ "$ROCKYLINUX_NINE" = '9' ]]; then
    echo "*************************************************"
    cecho "* Rocky Linux 9.x detected installing EPEL Repo" $boldgreen
    echo "*************************************************"
  elif [[ "$CENTOS_NINE" = '9' ]]; then
    echo "*************************************************"
    cecho "* CentOS 9.x detected installing EPEL Repo" $boldgreen
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
    yumpriorepel_nine
  elif [ ! -f /etc/yum.repos.d/epel.repo ]; then
    yum -y install epel-release
    ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
      cecho "Error: EPEL repo install failed." $boldgreen
      FAILEPEL='y'
      checklogdetails
    elif [[ "$ERROR" = '0' ]]; then
      yumpriorepel_nine
      echo
    fi
  fi

  cd $DIR_TMP
  
  if [ ! "$(yum repolist percona | grep -o percona)" ]; then
    echo "Configuring Percona EL9 repo"
    yum -y install https://repo.percona.com/yum/percona-release-latest.noarch.rpm
    ERR=$?
    CCPERCONACHECK="$ERR"
      if [ "$CCPERCONACHECK" == '0' ]; then
        yumpriorpercona_nine
        percona-release show
        percona-release disable all
        percona-release enable tools
        echo
      else
        cecho "Error: problem with Percona repo install." $boldgreen
      fi  
  fi
  
  if [[ "$(uname -m)" != 'aarch64' ]]; then
    reminineyum
  fi

  # CentOS 9 repo mirrors aren't fully updated so need to specify actual kernel-headers version
  PACKAGEEXTRA='perl-FindBin oniguruma5php oniguruma5php-devel figlet moreutils nghttp2 libnghttp2 libnghttp2-devel jq clang clang-devel jemalloc jemalloc-devel pngquant optipng jpegoptim pwgen pigz pbzip2 xz pxz lz4 libc-client libc-client-devel uw-imap-devel libtidy libtidy-devel libJudy axel bash-completion mlocate re2c libmcrypt libmcrypt-devel kernel-headers kernel-devel cmake'

  declare -a arrays
  arrays=(${PACKAGEEXTRA})

# if curl one liner is used and detected should skip this yum install routine
# since it's already done by curl installer script
if [ ! -f /usr/local/src/curlinstall_yum.txt ]; then

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
      else
        ${YUMDNFBIN}${CACHESKIP} -q -y install $pkgs --skip-broken
      fi
        # ${YUMDNFBIN}${CACHESKIP} -q list $pkgs
echo "----------------------------------------------------------------------------------"
        fi
done

fi # /usr/local/src/curlinstall_yum.txt

} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_yuminstall_centos9.log

fi # CENTOSVER = 8

}
