installpythonfuct() {

#ASK "Install Python Update ? [y/n] "
if [[ "$PYTHONUPDATE" = [yY] ]];
then
    echo "*************************************************"
    cecho "* Installing Python... " $boldgreen
    echo "*************************************************"
    echo "Installing..."

if [[ ! -f /usr/bin/python-config ]]; then
	yum -y install python-devel python-paramiko python-recaptcha-client
fi

cd $DIR_TMP

#download python tarball
# Feb 28, 2014 changed install method to outlined one at
# http://toomuchdata.com/2014/02/16/how-to-install-python-on-centos/

cecho "Compiling Python 2.7..." $boldgreen


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

python2.7 ez_setup.py
easy_install-2.7 pip
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
pip2.7 install virtualenv
pip2.7 install Django
pip2.7 install django-debug-toolbar
pip2.7 install Django-MPTT
pip2.7 install Coffin
pip2.7 install django-haystack
pip2.7 install Jinja2
pip2.7 install reCAPTCHA-client

#######################################################
# check /etc/ld.so.conf.d/opt-python${PYTHON_VERSION}.conf

# if [[ ! -f /etc/ld.so.conf.d/opt-python${PYTHON_VERSION}.conf ]]; then

# touch /etc/ld.so.conf.d/opt-python${PYTHON_VERSION}.conf
# echo "/opt/python${PYTHON_VERSION}/lib/" >> /etc/ld.so.conf.d/opt-python${PYTHON_VERSION}.conf
# ldconfig

# fi # check /etc/ld.so.conf.d/opt-python${PYTHON_VERSION}.conf

# ln -sf /opt/python${PYTHON_VERSION}/bin/python /usr/bin/python2.7

# cd $DIR_TMP

# sh setuptools-0.6c11-py2.7.egg --prefix=/opt/python${PYTHON_VERSION}

# cecho "/opt/python${PYTHON_VERSION}/bin/easy_install pip" $boldyellow

# /opt/python${PYTHON_VERSION}/bin/easy_install pip

# ln -sf /opt/python${PYTHON_VERSION}/bin/pip /usr/bin/pip
# ln -sf /opt/python${PYTHON_VERSION}/bin/virtualenv /usr/bin/virtualenv

#######################################################
# check python alias setup

# PYTHONALIASCHECK=`grep 'alias python=/opt/python2.7.3/bin/python' ~/.bash_profile`

# if [[ -z $PYTHONALIASCHECK ]]; then

# echo "alias python=/opt/python${PYTHON_VERSION}/bin/python" >> ~/.bash_profile
# echo "alias python2.7=/opt/python${PYTHON_VERSION}/bin/python" >> ~/.bash_profile
# echo "PATH=$PATH:/opt/python2.7/bin" >> ~/.bash_profile
# source ~/.bash_profile

# fi # check python alias setup

#cd /opt/python2.7.3/bin
#./easy_install ElementTree
#./easy_install Markdown
#./easy_install html5lib
#./easy_install python-openid

    echo "*************************************************"
    cecho "* Python Update installed " $boldgreen
    echo "*************************************************"
fi
fi


}