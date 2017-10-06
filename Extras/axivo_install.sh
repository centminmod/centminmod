#!/bin/bash
######################################################
# axivo yum repo manual install
# written by George Liu (eva2000) vbtechsupport.com
######################################################
# variables
#############
DT=$(date +"%d%m%y-%H%M%S")

WGETOPT='-cnv --no-dns-cache -4'
######################################################

TESTEDCENTOSVER='7.0'
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
source "../inc/centminlogs.inc"
source "../inc/yumskip.inc"
source "../inc/questions.inc"
source "../inc/downloads_centosfive.inc"
source "../inc/downloads_centossix.inc"
source "../inc/downloads_centosseven.inc"
source "../inc/downloadlinks.inc"
source "../inc/downloads.inc"
source "../inc/yumpriorities.inc"
source "../inc/yuminstall.inc"
source "../inc/centoscheck.inc"
source "../inc/axelsetup.inc"
######################################################
# functions
#############

if [[ "$CENTOS_SIX" = '6' ]]; then
{

if [ ${MACHINE_TYPE} == 'x86_64' ]; then

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
	exit #$ERROR
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
			exit $ERR
		fi
	fi

fi

fi

} 2>&1 | tee ${CENTMINLOGDIR}/axivo_install_${DT}_centos6.log
fi

if [[ "$CENTOS_SEVEN" = '7' ]]; then
{
if [ ${MACHINE_TYPE} == 'x86_64' ]; then

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
  exit #$ERROR
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
      exit $ERR
    fi
  fi

fi

fi

} 2>&1 | tee ${CENTMINLOGDIR}/axivo_install_${DT}_centos7.log
fi


######################################################
