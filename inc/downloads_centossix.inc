###############################################
atrpmssixyum() {

if [[ "$ATRPMSREPO_DISABLE" = [nN] ]]; then

# if [ ${MACHINE_TYPE} == 'x86_64' ]; then
    cd $DIR_TMP

  if [ -s "${CENTOSSIXATRPMSFILE}" ]; then
    echo "${CENTOSSIXATRPMSFILE} [found]"
    rpm --import http://packages.atrpms.net/RPM-GPG-KEY.atrpms
    rpm -Uvh ${CENTOSSIXATRPMSFILE}
    sed -i 's|enabled=1|enabled=0|g' /etc/yum.repos.d/atrpms.repo
    ERR=$?
    CCATRPMSCHECK="$ERR"
      if [ "$CCATRPMSCHECK" == '0' ]; then
        yumprioratrpms
        echo ""
      else
        cecho "Error: problem with ${CENTOSSIXATRPMSFILE} install." $boldgreen
        # exit $ERR
      fi    
    else
    echo "Error: ${CENTOSSIXATRPMSFILE} not found !!! Downloading now......"
    wget ${WGETOPT} ${CENTOSSIXATRPMS} --tries=2 --timeout=3
    # wget ${WGETOPT} ${CENTOSSIXATRPMSLOCAL} --tries=3
  ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
    cecho "Error: ${CENTOSSIXATRPMSFILE} download failed." $boldgreen
  FAILATRPMS='y'
  checklogdetails
    # exit #$ERROR
  else 
    cecho "Download done." $boldyellow
    rpm --import http://packages.atrpms.net/RPM-GPG-KEY.atrpms
    rpm -Uvh ${CENTOSSIXATRPMSFILE}
    sed -i 's|enabled=1|enabled=0|g' /etc/yum.repos.d/atrpms.repo
    ERR=$?
    CCATRPMSCHECK="$ERR"
      if [ "$CCATRPMSCHECK" == '0' ]; then
        yumprioratrpms
        echo ""
      else
        cecho "Error: problem with ${CENTOSSIXATRPMSFILE} install." $boldgreen
        # exit $ERR
      fi
    fi
  
  fi

# fi # MACHINE_TYPE

fi # ATRPMSREPO_DISABLE

}

###############################################
axivosixyum() {

if [[ "$AXIVOREPO_DISABLE" = [nN] ]]; then

if [ ${MACHINE_TYPE} == 'x86_64' ]; then

    cd $DIR_TMP 

if [ -s "${CENTOSSIXAXIVOFILE}" ]; then
  echo "${CENTOSSIXAXIVOFILE} [found]"
  else
  echo "Error: ${CENTOSSIXAXIVOFILE} not found !!! Downloading now......"
  wget ${WGETOPT} ${CENTOSSIXAXIVO} --tries=3
  # wget ${WGETOPT} ${CENTOSSIXAXIVOLOCAL} --tries=3
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${CENTOSSIXAXIVOFILE} download failed." $boldgreen
FAILAXIVO='y'
checklogdetails
  # exit #$ERROR
else 
  cecho "Download done." $boldyellow
  rpm -Uvh --nosignature ${CENTOSSIXAXIVOFILE}
  ERR=$?
  CCAXIVOCHECK="$ERR"
    if [ "$CCAXIVOCHECK" == '0' ]; then
      yumprioraxivo
      echo ""
    else
      cecho "Error: problem with ${CENTOSSIXAXIVOFILE} install." $boldgreen
      # exit $ERR
    fi
  fi

fi

fi

fi # AXIVOREPO_DISABLE

}

###############################################

remisixyum() {

if [[ "$REMIREPO_DISABLE" = [nN] ]]; then

# if [ ${MACHINE_TYPE} == 'x86_64' ]; then
    cd $DIR_TMP

  if [ -s "${CENTOSSIXREMIFILE}" ]; then
    echo "${CENTOSSIXREMIFILE} [found]"
    rpm --import http://rpms.remirepo.net/RPM-GPG-KEY-remi
    rpm --import https://rpms.remirepo.net/RPM-GPG-KEY-remi2017
    rpm -q gpg-pubkey --qf "%{summary} ->%{version}-%{release}\n"
    rpm -Uvh --nosignature ${CENTOSSIXREMIFILE}
    ERR=$?
    CCREMICHECK="$ERR"
      if [ "$CCREMICHECK" == '0' ]; then
        yumpriorremi
        echo ""
      else
        cecho "Error: problem with ${CENTOSSIXREMIFILE} install." $boldgreen
        # exit $ERR
      fi    
    else
    echo "Error: ${CENTOSSIXREMIFILE} not found !!! Downloading now......"
    wget ${WGETOPT} ${CENTOSSIXREMI} --tries=3
    # wget ${WGETOPT} ${CENTOSSIXREMILOCAL} --tries=3
  ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
    cecho "Error: ${CENTOSSIXREMIFILE} download failed." $boldgreen
  FAILREMI='y'
  checklogdetails
    # exit #$ERROR
  else 
    cecho "Download done." $boldyellow
    rpm --import http://rpms.remirepo.net/RPM-GPG-KEY-remi
    rpm --import https://rpms.remirepo.net/RPM-GPG-KEY-remi2017
    rpm -q gpg-pubkey --qf "%{summary} ->%{version}-%{release}\n"
    rpm -Uvh --nosignature ${CENTOSSIXREMIFILE}
    ERR=$?
    CCREMICHECK="$ERR"
      if [ "$CCREMICHECK" == '0' ]; then
        yumpriorremi
        echo ""
      else
        cecho "Error: problem with ${CENTOSSIXREMIFILE} install." $boldgreen
        # exit $ERR
      fi
    fi
  
  fi

# fi # MACHINE_TYPE

fi # REMIREPO_DISABLE

}

###############################################
yumcentossixdownload() {

if [[ "$CENTOS_SIX" = '6' ]]; then
{
echo "*************************************************"
cecho "* CentOS 6.x detected installing EPEL Repo" $boldgreen
echo "*************************************************"

    if [ ${MACHINE_TYPE} == 'x86_64' ];
    then
        ARCH='x86_64'
       RPMFARCH='x86_64'
    else
        ARCH='i686'
        RPMFARCH='i386'
    fi

    cd $DIR_TMP

if [ -f /etc/yum.repos.d/epel.repo ]; then
  yumpriorepel
elif [ ! -f /etc/yum.repos.d/epel.repo ]; then
  if [ -s "${CENTOSSIXEPELFILE}" ]; then
    echo "${CENTOSSIXEPELFILE} [found]"
    else
    echo "Error: ${CENTOSSIXEPELFILE} not found !!! Downloading now......"
    wget ${WGETOPT} ${CENTOSSIXEPEL} --tries=3
    # wget ${WGETOPT} ${CENTOSSIXEPELLOCAL} --tries=3
  ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
    cecho "Error: ${CENTOSSIXEPELFILE} download failed." $boldgreen
  FAILEPEL='y'
  checklogdetails
    # exit #$ERROR
  else 
    cecho "Download done." $boldyellow
    rpm --import http://epel.mirror.constant.com/RPM-GPG-KEY-EPEL-6
    rpm -Uvh ${CENTOSSIXEPELFILE}
    #yum${CACHESKIP} -q list ccache 2>&1 >/dev/null
    ERR=$?
    CCEPELCHECK="$ERR"
      if [ "$CCEPELCHECK" == '0' ]; then
        yumpriorepel
        echo ""
      else
        cecho "Error: problem with ${CENTOSSIXEPELFILE} install." $boldgreen
        # exit $ERR
      fi
    fi
  
  fi
fi

if [[ "$RPMFORGEREPO_DISABLE" = [nN] ]]; then
  if [[ -f /etc/yum.repos.d/rpmforge.repo || -f /etc/yum.repos.d/rpmforge.OLD ]]; then
    rm -rf /etc/yum.repos.d/rpmforge.repo
    rm -rf /etc/yum.repos.d/rpmforge.OLD
    # rm -rf ${DIR_TMP}/rpmforge-release*
  
    FORGERPM=$(rpm -qa | grep rpmforge-release)
    rpm -e $FORGERPM
  fi

  cd $DIR_TMP
  
  if [ -s "${CENTOSSIXRPMFORGEFILE}" ]; then
    echo "${CENTOSSIXRPMFORGEFILE} [found]"
  rpm --import http://apt.sw.be/RPM-GPG-KEY.dag.txt
  rpm -Uvh ${CENTOSSIXRPMFORGEFILE}
    ERR=$?
    CCRPMFORGECHECK="$ERR"
      if [ "$CCRPMFORGECHECK" == '0' ]; then
        yumpriorrpmforge
        echo ""
      else
        cecho "Error: problem with ${CENTOSSIXRPMFORGEFILE} install." $boldgreen
        #exit $ERR
      fi    
    else
    echo "Error: ${CENTOSSIXRPMFORGEFILE} not found !!! Downloading now......"
    wget ${WGETOPT} ${CENTOSSIXRPMFORGE} --tries=3 
  ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
    cecho "Error: ${CENTOSSIXRPMFORGEFILE} download failed." $boldgreen
  FAILRPMFORGE='y'
  checklogdetails
    #exit #$ERROR
  else 
          cecho "Download done." $boldyellow
  rpm --import http://apt.sw.be/RPM-GPG-KEY.dag.txt
  rpm -Uvh ${CENTOSSIXRPMFORGEFILE}
    # yum${CACHESKIP} -q list axel 2>&1 >/dev/null
    ERR=$?
    CCRPMFORGECHECK="$ERR"
      if [ "$CCRPMFORGECHECK" == '0' ]; then
        yumpriorrpmforge
        echo ""
      else
        cecho "Error: problem with ${CENTOSSIXRPMFORGEFILE} install." $boldgreen
        #exit $ERR
      fi
    fi
  
  fi
fi # RPMFORGEREPO_DISABLE = n

if [[ "$CENTALTREPO_DISABLE" = [nN] ]]; then
if [ -s "${CENTOSSIXCENTALTFILE}" ]; then
  echo "${CENTOSSIXCENTALTFILE} [found]"
  else
  echo "Error: ${CENTOSSIXCENTALTFILE} not found !!! Downloading now......"
  wget ${WGETOPT} ${CENTOSSIXCENTALT} --tries=3
  # wget ${WGETOPT} ${CENTOSSIXCENTALTLOCAL} --tries=3
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${CENTOSSIXCENTALTFILE} download failed." $boldgreen
FAILCENTALT='y'
checklogdetails
  # exit #$ERROR
else 
         cecho "Download done." $boldyellow
rpm --import http://centos.alt.ru/pub/repository/centos/RPM-GPG-KEY-CentALT
rpm -Uvh ${CENTOSSIXCENTALTFILE}
yumpriorcentalt
echo ""
  fi

fi
fi # CENTALTREPO_DISABLE

  cd $DIR_TMP

if [ -s "${CENTOSSIXPERCONALRELEASEFILE}" ]; then
  echo "${CENTOSSIXPERCONALRELEASEFILE} [found]"
  rpm --import https://www.percona.com/downloads/RPM-GPG-KEY-percona
  rpm -Uvh ${CENTOSSIXPERCONALRELEASEFILE}
  ERR=$?
  CCPERCONACHECK="$ERR"
    if [ "$CCPERCONACHECK" == '0' ]; then
      yumpriorpercona
      echo ""
    else
      cecho "Error: problem with ${CENTOSSIXPERCONALRELEASEFILE} install." $boldgreen
      exit $ERR
    fi  
  else
  echo "Error: ${CENTOSSIXPERCONALRELEASEFILE} not found !!! Downloading now......"
  wget ${WGETOPT} ${CENTOSSIXPERCONALRELEASE} --tries=3 
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${CENTOSSIXPERCONALRELEASEFILE} download failed." $boldgreen
FAILPERCONA='y'
checklogdetails
  # exit #$ERROR
else 
         cecho "Download done." $boldyellow
rpm --import https://www.percona.com/downloads/RPM-GPG-KEY-percona
rpm -Uvh ${CENTOSSIXPERCONALRELEASEFILE}
  #yum -q list axel 2>&1 >/dev/null
  ERR=$?
  CCPERCONACHECK="$ERR"
    if [ "$CCPERCONACHECK" == '0' ]; then
      yumpriorpercona
      echo ""
    else
      cecho "Error: problem with ${CENTOSSIXPERCONALRELEASEFILE} install." $boldgreen
      exit $ERR
    fi
  fi

fi

  if [[ "$(uname -m)" = 'x86_64' ]]; then
    varnishfour_setup
    yumpriorvarnish
    if [[ "$VARNISHREPO_DISABLE" = [nN] && "$INITIALINSTALL" = [yY] && -f "$VARNISH_REPOFILE" ]]; then
      # disable for install stage to speed up install
      yum-config-manager --disable varnishcache_varnish41 varnishcache_varnish41-source
    fi
  fi # 64bit only

####
# axivo centos 6.x 64bit only
# axivosixyum
####
if [[ "$(uname -m)" != 'aarch64' ]]; then
  remisixyum
  atrpmssixyum
fi

# CentOS 6 repo mirrors aren't fully updated so need to specify actual kernel-headers version
PACKAGEEXTRA='figlet moreutils nghttp2 libnghttp2 libnghttp2-devel clang clang-devel jemalloc jemalloc-devel pngquant optipng jpegoptim pwgen pigz pbzip2 xz pxz lz4 libJudy axel glances bash-completion bash-completion-extras mlocate re2c libmcrypt libmcrypt-devel kernel-headers kernel-devel cmake28'

declare -a arrays
arrays=(${PACKAGEEXTRA})

# if curl one liner is used and detected should skip this yum install routine
# since it's already done by curl installer script
if [ ! -f /usr/local/src/curlinstall_yum.txt ]; then

for pkgs in ${arrays[@]}
do
echo Processing package: $pkgs
YUMINSTALLRPMCHECK=$(rpm -ql $pkgs >/dev/null 2>&1; echo $?)
#echo $YUMINSTALLRPMCHECK
        if [[ "$YUMINSTALLRPMCHECK" = '0' ]]; then
echo "----------------------------------------------------------------------------------"
        # yum${CACHESKIP} -q -y update $pkgs
        # yum${CACHESKIP} -q list $pkgs
        echo "$pkgs already installed"
echo "----------------------------------------------------------------------------------"
        else
echo "----------------------------------------------------------------------------------"
cecho "Installing $pkgs" $boldyellow
echo "----------------------------------------------------------------------------------"
      if [[ "$pkgs" = 'libzip5' || "$pkgs" = 'libzip5-devel' ]]; then
        ${YUMDNFBIN}${CACHESKIP} -q -y install $pkgs --enablerepo=remi
      else
        ${YUMDNFBIN}${CACHESKIP} -q -y install $pkgs
      fi
        # yum${CACHESKIP} -q list $pkgs
echo "----------------------------------------------------------------------------------"
        fi
done

fi # /usr/local/src/curlinstall_yum.txt

} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_yuminstall_centos6.log
fi

}