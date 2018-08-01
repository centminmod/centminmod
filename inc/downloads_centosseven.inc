###############################################
atrpmssevenyum() {

if [[ "$ATRPMSREPO_DISABLE" = [nN] ]]; then

# if [ ${MACHINE_TYPE} == 'x86_64' ]; then
    cd $DIR_TMP

  if [ -s "${CENTOSSEVENATRPMSFILE}" ]; then
    echo "${CENTOSSEVENATRPMSFILE} [found]"
    rpm --import http://packages.atrpms.net/RPM-GPG-KEY.atrpms
    rpm -Uvh ${CENTOSSEVENATRPMSFILE}
    sed -i 's|enabled=1|enabled=0|g' /etc/yum.repos.d/atrpms.repo
    ERR=$?
    CCATRPMSCHECK="$ERR"
      if [ "$CCATRPMSCHECK" == '0' ]; then
        yumprioratrpms
        echo ""
      else
        cecho "Error: problem with ${CENTOSSEVENATRPMSFILE} install." $boldgreen
        # exit $ERR
      fi    
    else
    echo "Error: ${CENTOSSEVENATRPMSFILE} not found !!! Downloading now......"
    wget ${WGETOPT} ${CENTOSSEVENATRPMS} --tries=2 --timeout=3
    # wget ${WGETOPT} ${CENTOSSEVENATRPMSLOCAL} --tries=3
  ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
    cecho "Error: ${CENTOSSEVENATRPMSFILE} download failed." $boldgreen
  FAILATRPMS='y'
  checklogdetails
    # exit #$ERROR
  else 
    cecho "Download done." $boldyellow
    rpm --import http://packages.atrpms.net/RPM-GPG-KEY.atrpms
    rpm -Uvh ${CENTOSSEVENATRPMSFILE}
    sed -i 's|enabled=1|enabled=0|g' /etc/yum.repos.d/atrpms.repo
    ERR=$?
    CCATRPMSCHECK="$ERR"
      if [ "$CCATRPMSCHECK" == '0' ]; then
        yumprioratrpms
        echo ""
      else
        cecho "Error: problem with ${CENTOSSEVENATRPMSFILE} install." $boldgreen
        # exit $ERR
      fi
    fi
  
  fi

# fi # MACHINE_TYPE

fi # ATRPMSREPO_DISABLE

}

###############################################
axivoseven() {

if [[ "$AXIVOREPO_DISABLE" = [nN] ]]; then

if [ ${MACHINE_TYPE} == 'x86_64' ]; then

    cd $DIR_TMP  

if [ -s "${CENTOSSEVENAXIVOFILE}" ]; then
  echo "${CENTOSSEVENAXIVOFILE} [found]"
  else
  echo "Error: ${CENTOSSEVENAXIVOFILE} not found !!! Downloading now......"
  wget ${WGETOPT} ${CENTOSSEVENAXIVO} --tries=3
  # wget ${WGETOPT} ${CENTOSSEVENAXIVOLOCAL} --tries=3
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${CENTOSSEVENAXIVOFILE} download failed." $boldgreen
FAILAXIVO='y'
checklogdetails
  # exit #$ERROR
else 
  cecho "Download done." $boldyellow
  rpm -Uvh --nosignature ${CENTOSSEVENAXIVOFILE}
  ERR=$?
  CCAXIVOCHECK="$ERR"
    if [ "$CCAXIVOCHECK" == '0' ]; then
      yumprioraxivo
      echo ""
    else
      cecho "Error: problem with ${CENTOSSEVENAXIVOFILE} install." $boldgreen
      # exit $ERR
    fi
  fi

fi

fi

fi # AXIVOREPO_DISABLE

}

###############################################

remisevenyum() {

if [[ "$REMIREPO_DISABLE" = [nN] ]]; then

# if [ ${MACHINE_TYPE} == 'x86_64' ]; then
    cd $DIR_TMP

  if [ -s "${CENTOSSEVENREMIFILE}" ]; then
    echo "${CENTOSSEVENREMIFILE} [found]"
    rpm --import http://rpms.remirepo.net/RPM-GPG-KEY-remi
    rpm --import https://rpms.remirepo.net/RPM-GPG-KEY-remi2017
    rpm -q gpg-pubkey --qf "%{summary} ->%{version}-%{release}\n"
    rpm -Uvh --nosignature ${CENTOSSEVENREMIFILE}
    ERR=$?
    CCREMICHECK="$ERR"
      if [ "$CCREMICHECK" == '0' ]; then
        yumpriorremi
        echo ""
      else
        cecho "Error: problem with ${CENTOSSEVENREMIFILE} install." $boldgreen
        # exit $ERR
      fi    
    else
    echo "Error: ${CENTOSSEVENREMIFILE} not found !!! Downloading now......"
    wget ${WGETOPT} ${CENTOSSEVENREMI} --tries=3
    # wget ${WGETOPT} ${CENTOSSEVENREMILOCAL} --tries=3
  ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
    cecho "Error: ${CENTOSSEVENREMIFILE} download failed." $boldgreen
  FAILREMI='y'
  checklogdetails
    # exit #$ERROR
  else 
    cecho "Download done." $boldyellow
    rpm --import http://rpms.remirepo.net/RPM-GPG-KEY-remi
    rpm --import https://rpms.remirepo.net/RPM-GPG-KEY-remi2017
    rpm -q gpg-pubkey --qf "%{summary} ->%{version}-%{release}\n"
    rpm -Uvh --nosignature ${CENTOSSEVENREMIFILE}
    ERR=$?
    CCREMICHECK="$ERR"
      if [ "$CCREMICHECK" == '0' ]; then
        yumpriorremi
        echo ""
      else
        cecho "Error: problem with ${CENTOSSEVENREMIFILE} install." $boldgreen
        # exit $ERR
      fi
    fi
  
  fi

# fi # MACHINE_TYPE

fi # REMIREPO_DISABLE

}

###############################################
yumcentossevendownload() {

if [[ "$CENTOS_SEVEN" = '7' ]]; then
{
echo "*************************************************"
cecho "* CentOS 7.x detected installing EPEL Repo" $boldgreen
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
  if [ -s "${CENTOSSEVENEPELFILE}" ]; then
    echo "${CENTOSSEVENEPELFILE} [found]"
    else
    echo "Error: ${CENTOSSEVENEPELFILE} not found !!! Downloading now......"
    wget ${WGETOPT} ${CENTOSSEVENEPEL} --tries=3
    # wget ${WGETOPT} ${CENTOSSEVENEPELLOCAL} --tries=3
  ERROR=$?
   if [[ "$ERROR" != '0' ]]; then
   cecho "Error: ${CENTOSSEVENEPELFILE} download failed." $boldgreen
  FAILEPEL='y'
  checklogdetails
   # exit #$ERROR
  else 
   cecho "Download done." $boldyellow
    rpm --import http://epel.mirror.constant.com/RPM-GPG-KEY-EPEL-7
    rpm -Uvh ${CENTOSSEVENEPELFILE}
   #${YUMDNFBIN}${CACHESKIP} -q list ccache 2>&1 >/dev/null
   ERR=$?
   CCEPELCHECK="$ERR"
      if [ "$CCEPELCHECK" == '0' ]; then
       yumpriorepel
       echo ""
      else
       cecho "Error: problem with ${CENTOSSEVENEPELFILE} install." $boldgreen
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
  
  if [ -s "${CENTOSSEVENRPMFORGEFILE}" ]; then
    echo "${CENTOSSEVENRPMFORGEFILE} [found]"
  rpm --import http://apt.sw.be/RPM-GPG-KEY.dag.txt
  rpm -Uvh ${CENTOSSEVENRPMFORGEFILE}    
    ERR=$?
    CCRPMFORGECHECK="$ERR"
      if [ "$CCRPMFORGECHECK" == '0' ]; then
        yumpriorrpmforge
        echo ""
      else
        cecho "Error: problem with ${CENTOSSEVENRPMFORGEFILE} install." $boldgreen
        #exit $ERR
      fi  
    else
    echo "Error: ${CENTOSSEVENRPMFORGEFILE} not found !!! Downloading now......"
    wget ${WGETOPT} ${CENTOSSEVENRPMFORGE} --tries=3 
  ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
    cecho "Error: ${CENTOSSEVENRPMFORGEFILE} download failed." $boldgreen
  FAILRPMFORGE='y'
  checklogdetails
    #exit #$ERROR
  else 
          cecho "Download done." $boldyellow
  rpm --import http://apt.sw.be/RPM-GPG-KEY.dag.txt
  rpm -Uvh ${CENTOSSEVENRPMFORGEFILE}
    # ${YUMDNFBIN}${CACHESKIP} -q list axel 2>&1 >/dev/null
    ERR=$?
    CCRPMFORGECHECK="$ERR"
      if [ "$CCRPMFORGECHECK" == '0' ]; then
        yumpriorrpmforge
        echo ""
      else
        cecho "Error: problem with ${CENTOSSEVENRPMFORGEFILE} install." $boldgreen
        #exit $ERR
      fi
    fi
  
  fi
fi # RPMFORGEREPO_DISABLE = n


if [[ "$CENTALTREPO_DISABLE" = [nN] ]]; then
if [ -s "${CENTOSSEVENCENTALTFILE}" ]; then
  echo "${CENTOSSEVENCENTALTFILE} [found]"
  else
  echo "Error: ${CENTOSSEVENCENTALTFILE} not found !!! Downloading now......"
  wget ${WGETOPT} ${CENTOSSEVENCENTALT} --tries=3
  # wget ${WGETOPT} ${CENTOSSEVENCENTALTLOCAL} --tries=3
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${CENTOSSEVENCENTALTFILE} download failed." $boldgreen
FAILCENTALT='y'
checklogdetails
  # exit #$ERROR
else 
         cecho "Download done." $boldyellow
rpm --import http://centos.alt.ru/pub/repository/centos/RPM-GPG-KEY-CentALT
rpm -Uvh ${CENTOSSEVENCENTALTFILE}
yumpriorcentalt
echo ""
  fi

fi
fi # CENTALTREPO_DISABLE

  cd $DIR_TMP

if [ -s "${CENTOSSEVENPERCONALRELEASEFILE}" ]; then
  echo "${CENTOSSEVENPERCONALRELEASEFILE} [found]"
  rpm --import https://www.percona.com/downloads/RPM-GPG-KEY-percona
  rpm -Uvh ${CENTOSSEVENPERCONALRELEASEFILE}  
  ERR=$?
  CCPERCONACHECK="$ERR"
    if [ "$CCPERCONACHECK" == '0' ]; then
      yumpriorpercona
      echo ""
    else
      cecho "Error: problem with ${CENTOSSEVENPERCONALRELEASEFILE} install." $boldgreen
      # exit $ERR
    fi  
  else
  echo "Error: ${CENTOSSEVENPERCONALRELEASEFILE} not found !!! Downloading now......"
  wget ${WGETOPT} ${CENTOSSEVENPERCONALRELEASE} --tries=3 
ERROR=$?
  if [[ "$ERROR" != '0' ]]; then
  cecho "Error: ${CENTOSSEVENPERCONALRELEASEFILE} download failed." $boldgreen
FAILPERCONA='y'
checklogdetails
  # exit #$ERROR
else 
         cecho "Download done." $boldyellow
rpm --import https://www.percona.com/downloads/RPM-GPG-KEY-percona
rpm -Uvh ${CENTOSSEVENPERCONALRELEASEFILE}
  #yum -q list axel 2>&1 >/dev/null
  ERR=$?
  CCPERCONACHECK="$ERR"
    if [ "$CCPERCONACHECK" == '0' ]; then
      yumpriorpercona
      echo ""
    else
      cecho "Error: problem with ${CENTOSSEVENPERCONALRELEASEFILE} install." $boldgreen
      # exit $ERR
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
# axivoseven
####
if [[ "$(uname -m)" != 'aarch64' ]]; then
  remisevenyum
  atrpmssevenyum
fi

# CentOS 7 repo mirrors aren't fully updated so need to specify actual kernel-headers version
PACKAGEEXTRA='figlet moreutils nghttp2 libnghttp2 libnghttp2-devel clang clang-devel jemalloc jemalloc-devel pngquant optipng jpegoptim pwgen pigz pbzip2 xz pxz lz4 uw-imap-devel libtidy libtidy-devel libJudy axel glances bash-completion bash-completion-extras mlocate re2c libmcrypt libmcrypt-devel kernel-headers kernel-devel cmake28'

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
        # ${YUMDNFBIN}${CACHESKIP} -q -y update $pkgs
        # ${YUMDNFBIN}${CACHESKIP} -q list $pkgs
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
        # ${YUMDNFBIN}${CACHESKIP} -q list $pkgs
echo "----------------------------------------------------------------------------------"
        fi
done

fi # /usr/local/src/curlinstall_yum.txt

} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_yuminstall_centos7.log

fi # CENTOSVER = 7

}
