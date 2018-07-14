install_axel() {
  AXELVERBOSE=$1

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

    if [[ "$AXELVERBOSE" = 'silent' ]]; then
      CONFIG_VERBOSEOPT=' -q'
      MAKE_VERBOSEOPT=' -s'
    else
      CONFIG_VERBOSEOPT=""
      MAKE_VERBOSEOPT=""
    fi
    if [[ -f "$(which figlet)" && "$AXELVERBOSE" != 'silent' ]]; then
        figlet -ckf standard "Axel Install (install_axel)"
    fi

  cd $DIR_TMP

        if [[ "$AXELVERBOSE" != 'silent' ]]; then
          cecho "Download $AXEL_LINKFILE ..." $boldyellow
        fi
    if [ -s $AXEL_LINKFILE ]; then
        cecho "Axel ${AXEL_VER} Archive found, skipping download..." $boldgreen 
    else
        wget -O $AXEL_LINKFILE $AXEL_LINK
ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
    cecho "Error: $AXEL_LINKFILE download failed." $boldgreen
checklogdetails
    exit #$ERROR
else 
         cecho "Download $AXEL_LINKFILE done." $boldyellow
#echo ""
    fi
    fi

if [[ "$(tar -tzf axel-${AXEL_VER}.tar.gz >/dev/null; echo $?)" != '0' ]]; then
  rm -rf /svr-setup/axel-${AXEL_VER}.*
  echo "re-try download form local mirror..."
  wget -O $AXEL_LINKFILE $AXEL_LINKLOCAL
fi

tar xzf $AXEL_LINKFILE
ERROR=$?
    if [[ "$ERROR" != '0' ]]; then
    cecho "Error: $AXEL_LINKFILE extraction failed." $boldgreen
checklogdetails
    exit #$ERROR
else 
         cecho "$AXEL_LINKFILE valid file." $boldyellow
echo ""
    fi

  cd axel-${AXEL_VER}
  if [[ "$AXELVERBOSE" != 'silent' ]]; then
    if [ -f autogen.sh ]; then
      ./autogen.sh
    fi
  else
    if [ -f autogen.sh ]; then
      ./autogen.sh >/dev/null 2>&1 
    fi
  fi
  ./configure${CONFIG_VERBOSEOPT}
  make${MAKETHREADS}${MAKE_VERBOSEOPT}
  make install${MAKE_VERBOSEOPT}
  if [[ "$AXELVERBOSE" != 'silent' ]]; then
   which axel
  fi
  if [[ "$AXELVERBOSE" != 'silent' ]]; then
    echo
    cecho "Updated Axel ${AXEL_VER} successfully..." $boldyellow
    echo
  else
    echo
    cecho "Installed Axel ${AXEL_VER} successfully..." $boldyellow
    echo
  fi
}

axelsetup() {

    # if [ -f "$(which figlet)" ]; then
    #     figlet -ckf standard "Axel Setup (axelsetup)"
    # fi

  if [[ "$INITIALINSTALL" = [yY] ]]; then
    # remove any yum installed axel packages
    # in favour of source compiled package
    if [[ -z "$(rpm -ql axel | grep 'not installed')" && -f /usr/local/bin/axel ]]; then
      yum -q -y remove axel
    fi
  elif [[ "$INITIALINSTALL" != [yY] ]]; then
    # remove any yum installed axel packages
    # in favour of source compiled package
    if [[ -z "$(rpm -ql axel | grep 'not installed')" && -f /usr/local/bin/axel ]]; then
      yum -q -y remove axel
    fi
  fi

  if [ ! -f /usr/local/bin/axel ]; then
    AXELRPMCHECK=0
    # echo
    # echo "Detected that axel download accelerator"
    # echo "was not installed via rpmforge repository"
    # echo
    # echo "Trying alternate mirror for required RPMs"
    # echo

  if [[ "$CENTOSVER" > 5 && "$CENTOSVER" < 6 ]]; then

      cd $DIR_TMP

    if [[ "$RPMFORGEREPO_DISABLE" = [nN] ]]; then
      if [ -f "${DIR_TMP}/rpmforge-release-0.5.3-1.el5.rf.${ARCH}.rpm" ]; then
        echo "rpmforge-release-0.5.3-1.el5.rf.${ARCH}.rpm [found]"
        else
        echo "Error: rpmforge-release-0.5.3-1.el5.rf.${ARCH}.rpm not found !!! Downloading now......"
        wget -${ipv_forceopt}cnv https://centminmod.com/centminmodparts/v123/04rpms/${ARCH}/rpmforge-release-0.5.3-1.el5.rf.${ARCH}.rpm --tries=3 
      ERROR=$?
        if [[ "$ERROR" != '0' ]]; then
        cecho "Error: rpmforge-release-0.5.3-1.el5.rf.${ARCH}.rpm download failed." $boldgreen
      FAILRPMFORGE='y'
      checklogdetails
        # exit $ERROR
      else 
              cecho "Download done." $boldyellow
      rpm --import http://apt.sw.be/RPM-GPG-KEY.dag.txt
      rpm -Uvh rpmforge-release-0.5.3-1.el5.rf.${ARCH}.rpm
        #yum -q list axel 2>&1 >/dev/null
        ERR=$?
        CCRPMFORGECHECK="$ERR"
          if [ "$CCRPMFORGECHECK" == '0' ]; then
            yumpriorrpmforge
            echo ""
          else
            cecho "Error: problem with rpmforge-release-0.5.3-1.el5.rf.${ARCH}.rpm install." $boldgreen
            exit $ERR
          fi
        fi
      
      fi
    fi # RPMFORGEREPO_DISABLE = n

    if [ -f "${DIR_TMP}/axel-2.4-1.el5.rf.${ARCH}.rpm" ]; then
      echo "axel-2.4-1.el5.rf.${ARCH}.rpm [found]"
      else
      echo "Error: axel-2.4-1.el5.rf.${ARCH}.rpm not found !!! Downloading now......"
      wget -${ipv_forceopt}cnv https://centminmod.com/centminmodparts/v123/04rpms/${ARCH}/axel-2.4-1.el5.rf.${ARCH}.rpm --tries=3 
    ERROR=$?
      if [[ "$ERROR" != '0' ]]; then
      cecho "Error: axel-2.4-1.el5.rf.${ARCH}.rpm download failed." $boldgreen
    FAILEDAXELRPM='y'
    checklogdetails
      exit $ERROR
    else 
            cecho "Download done." $boldyellow
    rpm -Uvh axel-2.4-1.el5.rf.${ARCH}.rpm
      ERR=$?
      AXELFORGECHECK="$ERR"
        if [ "$AXELFORGECHECK" == '0' ]; then
          #yumpriorrpmforge
          echo ""
        else
          cecho "Error: problem with axel-2.4-1.el5.rf.${ARCH}.rpm install." $boldgreen
          exit $ERR
        fi
      fi
    fi
  fi # centos 5.x ver

  ###
  if [[ "$CENTOS_SIX" = '6' && ! -f /usr/local/bin/axel ]]; then
    install_axel
  fi
  ###

  if [[ "$CENTOS_SEVEN" = '7' && ! -f /usr/local/bin/axel ]]; then
    # axel download accelerator works with CentOS 7.x and epel repo
    # unlike axel provided by rpmforge on CentOS 6.x repo
    # yum -y install axel
    install_axel
  fi
  fi # check for /usr/bin/axel
}