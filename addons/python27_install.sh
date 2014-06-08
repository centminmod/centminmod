#!/bin/bash
VER='0.0.1'
######################################################
# python 2.7 installer for Centminmod.com
# written by George Liu (eva2000) vbtechsupport.com
######################################################
PYTHON_VERSION=2.7.7

######################################################
DT=`date +"%d%m%y-%H%M%S"`
CENTMINLOGDIR='/root/centminlogs'
DIR_TMP='/svr-setup'

PYTHON_LINKFILE="Python-${PYTHON_VERSION}.tgz"
PYTHON_LINK="http://www.python.org/ftp/python/${PYTHON_VERSION}/${PYTHON_LINKFILE}"

EZSETUPLINKFILE='ez_setup.py'
EZSETUPLINK="https://bitbucket.org/pypa/setuptools/raw/bootstrap/${EZSETUPLINKFILE}"
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
pythontarball() {

    cd $DIR_TMP

        cecho "Download ${PYTHON_LINKFILE} ..." $boldyellow
if [ -s ${PYTHON_LINKFILE} ]; then
  cecho "${PYTHON_LINKFILE} found, skipping download..." $boldgreen
  else
  echo "Error: ${PYTHON_LINKFILE} not found !!! Download now......"
        wget -cnv http://www.python.org/ftp/python/${PYTHON_VERSION}/${PYTHON_LINKFILE} --tries=3
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "${PYTHON_LINKFILE} download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download done." $boldyellow
#echo ""
	fi
fi

tar xzf ${PYTHON_LINKFILE} 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${PYTHON_LINKFILE} extraction failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "${PYTHON_LINKFILE} valid file." $boldyellow
echo ""
	fi

        cecho "Download ${PYTHON_SETUPTOOLSLINKFILE} ..." $boldyellow
if [ -s ${PYTHON_SETUPTOOLSLINKFILE} ]; then
  cecho "${PYTHON_SETUPTOOLSLINKFILE} found, skipping download..." $boldgreen
  else
  echo "Error: ${PYTHON_SETUPTOOLSLINKFILE} not found !!! Download now......"
        wget -c --no-check-certificate ${PYTHON_SETUPTOOLSLINK} --tries=3 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "${PYTHON_SETUPTOOLSLINKFILE} download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download ${PYTHON_SETUPTOOLSLINKFILE} done." $boldyellow
echo ""
	fi
fi

}


installpythonfuct() {


    cecho "*************************************************" $boldyellow
    cecho "* Install Python 2.7 at /usr/local/bin/python2.7 ... " $boldgreen
    cecho "*************************************************" $boldyellow

cecho "Downloading..." $boldyellow

pythontarball

cecho "Installing..." $boldyellow

if [[ ! -f /usr/bin/python-config ]]; then
	yum -y install python-devel python-paramiko python-recaptcha-client
fi

cd $DIR_TMP

#download python tarball
# Feb 28, 2014 changed install method to outlined one at
# http://toomuchdata.com/2014/02/16/how-to-install-python-on-centos/

cecho "Compiling Python 2.7..." $boldyellow

#tar xvfz Python-${PYTHON_VERSION}.tgz
cd Python-${PYTHON_VERSION}
./configure --prefix=/usr/local --with-threads --enable-unicode=ucs4 --enable-shared LDFLAGS="-Wl,-rpath /usr/local/lib"
make
make altinstall

if [[ -f /usr/local/bin/python2.7 ]]; then

cd $DIR_TMP

        cecho "Download ${EZSETUPLINKFILE} ..." $boldyellow
    if [ -s ${EZSETUPLINKFILE} ]; then
        cecho "${EZSETUPLINKFILE} found, skipping download..." $boldgreen
    else
        wget $EZSETUPLINK --tries=3 
ERROR=$?
	if [[ "$ERROR" != '0' ]]; then
	cecho "Error: ${EZSETUPLINKFILE} download failed." $boldgreen
checklogdetails
	exit #$ERROR
else 
         cecho "Download ${EZSETUPLINKFILE} done." $boldyellow
#echo ""
	fi
    fi

echo "python2.7 ez_setup.py
easy_install-2.7 pip
pip2.7 install virtualenv
pip2.7 install uwsgi"

python2.7 ez_setup.py
easy_install-2.7 pip
pip2.7 install virtualenv
pip2.7 install uwsgi

virtualenvonly() {
easy_install-2.7 ElementTree
easy_install-2.7 Markdown
easy_install-2.7 html5lib
easy_install-2.7 python-openid
easy_install-2.7 requests
easy_install-2.7 psutil
easy_install-2.7 Pillow
easy_install-2.7 path.py
easy_install-2.7 pytz
easy_install-2.7 unidecode
easy_install-2.7 Whoosh
easy_install-2.7 South
pip2.7 install Django
pip2.7 install django-debug-toolbar
pip2.7 install Django-MPTT
pip2.7 install Coffin
pip2.7 install django-haystack
pip2.7 install Jinja2
pip2.7 install reCAPTCHA-client
}

echo

echo "---------------------"
cecho "python --version" $boldyellow
python --version
which python
which pip
which easy_install
echo "---------------------"
cecho "python2.7 --version" $boldyellow
python2.7 --version
which python2.7
which pip2.7
which easy_install-2.7
echo "---------------------"

    cecho "*************************************************" $boldyellow
    cecho "* python2.7 installed " $boldgreen
    cecho "*************************************************" $boldyellow
fi
}

###########################################################################
case $1 in
  install)
starttime=$(date +%s.%N)
{
  installpythonfuct
} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_python27_install_${DT}.log

endtime=$(date +%s.%N)

INSTALLTIME=$(echo "scale=2;$endtime - $starttime"|bc )
echo "" >> ${CENTMINLOGDIR}/centminmod_python27_install_${DT}.log
echo "Total python 2.7 Install Time: $INSTALLTIME seconds" >> ${CENTMINLOGDIR}/centminmod_python27_install_${DT}.log
  ;;
  *)
    echo "$0 install"
  ;;
esac
exit
