python_alternatives_setup() {
    # for EL8+ python unversion configuration
    # default to Python 3.6
    if [[ "$PYTHON_INSTALL_ALTERNATIVES" = [yY] && "$CENTOS_EIGHT" -eq '8' ]]; then
        if [ ! -f "/usr/bin/python${ALT_PYTHON_VER_LABEL}" ]; then
            yum -y install python${ALT_PYTHON_VER}
        fi
        if [[ ! "$(rpm -qa python${ALT_PYTHON_VER}-devel)" ]]; then
            yum -y install python${ALT_PYTHON_VER}-devel
        fi
        if [ ! -f "/usr/bin/pip${ALT_PYTHON_VER_LABEL}" ]; then
            yum -y install python3-pip
        fi
        if [[ ! "$(alternatives --list | grep -E 'manual.*\/usr\/bin\/python3')" && ! "$(alternatives --list | egrep -i 'pip|python' | grep -E 'manual.*\/usr\/bin\/pip3')" ]]; then
            echo
            echo "Setup python3 and pip3 alternatives mapping"
            echo "you can reference pip for pip3"
            echo "you can reference python for python3"
            alternatives --set python /usr/bin/python3
            alternatives --install /usr/bin/pip pip /usr/bin/pip3 1
            alternatives --set pip /usr/bin/pip3
            alternatives --list | egrep -i 'pip|python'
            echo
        else
            echo
            # echo "Skip python3 and pip3 alternatives mapping"
        fi
        if [[ "$(rpm -qa python3-numpy | grep -o 'python3-numpy')" != 'python3-numpy' ]]; then
            yum -y install python3-numpy
        fi
        if [[ "$(rpm -qa python3-requests | grep -o 'python3-requests')" != 'python3-requests' ]]; then
            yum -y install python3-requests
        fi
    elif [[ "$PYTHON_INSTALL_ALTERNATIVES" = [yY] && "$CENTOS_NINE" -eq '9' ]] && [[ ! -f /usr/bin/python3 && ! -f /usr/bin/pip ]]; then
        # ALT_PYTHON_VER_LABEL='3.11'
        ALT_PYTHON_VER_EL9='3.11'
        if [ ! -f "/usr/bin/python${ALT_PYTHON_VER_EL9}" ]; then
            yum -y install python${ALT_PYTHON_VER_EL9}
        fi
        if [[ ! "$(rpm -qa python${ALT_PYTHON_VER_EL9}-devel)" ]]; then
            yum -y install python${ALT_PYTHON_VER_EL9}-devel
        fi
        if [ ! -f "/usr/bin/pip${ALT_PYTHON_VER_EL9}" ]; then
            yum -y install python${ALT_PYTHON_VER_EL9}-pip
        fi
        if [[ ! "$(alternatives --list | grep -E 'manual.*\/usr\/bin\/python')" && ! "$(alternatives --list | egrep -i 'pip|python' | grep -E 'manual.*\/usr\/bin\/pip')" ]]; then
            echo
            echo "Setup python${ALT_PYTHON_VER_EL9} and pip${ALT_PYTHON_VER_EL9} alternatives mapping"
            echo "you can reference pip for pip${ALT_PYTHON_VER_EL9}"
            echo "you can reference python for python${ALT_PYTHON_VER_EL9}"
            alternatives --set python /usr/bin/python${ALT_PYTHON_VER_EL9}
            alternatives --install /usr/bin/pip pip /usr/bin/pip${ALT_PYTHON_VER_EL9} 1
            alternatives --set pip /usr/bin/pip${ALT_PYTHON_VER_EL9}
            alternatives --list | egrep -i 'pip|python'
            echo
        else
            echo
            echo "Skip python${ALT_PYTHON_VER_EL9} and pip${ALT_PYTHON_VER_EL9} alternatives mapping"
        fi
        if [[ "$(rpm -qa python3-numpy | grep -o 'python3-numpy')" != 'python3-numpy' ]]; then
            yum -y install python3-numpy
        fi
        if [[ "$(rpm -qa python3-requests | grep -o 'python3-requests')" != 'python3-requests' ]]; then
            yum -y install python3-requests
        fi
    fi
}

installpythonfuct() {

#ASK "Install Python Update ? [y/n] "
if [[ "$PYTHONUPDATE" = [yY] ]];
then
    echo "*************************************************"
    cecho "* Installing Python... " $boldgreen
    echo "*************************************************"
    echo "Installing..."

if [[ ! -f /usr/bin/python-config ]]; then
	time $YUMDNFBIN -y install python-devel python-paramiko python-recaptcha-client
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
PYTHONWARNINGS=ignore:::pip._internal.cli.base_command pip2.7 install virtualenv
PYTHONWARNINGS=ignore:::pip._internal.cli.base_command pip2.7 install Django
PYTHONWARNINGS=ignore:::pip._internal.cli.base_command pip2.7 install django-debug-toolbar
PYTHONWARNINGS=ignore:::pip._internal.cli.base_command pip2.7 install Django-MPTT
PYTHONWARNINGS=ignore:::pip._internal.cli.base_command pip2.7 install Coffin
PYTHONWARNINGS=ignore:::pip._internal.cli.base_command pip2.7 install django-haystack
PYTHONWARNINGS=ignore:::pip._internal.cli.base_command pip2.7 install Jinja2
PYTHONWARNINGS=ignore:::pip._internal.cli.base_command pip2.7 install reCAPTCHA-client

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