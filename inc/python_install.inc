installpythonfuct() {

#ASK "Install Python Update ? [y/n] "
if [[ "$PYTHONUPDATE" = [yY] ]];
then
    echo "*************************************************"
    cecho "* Installing Python... " $boldgreen
    echo "*************************************************"
    echo "Installing..."

cd $DIR_TMP

#download python tarball

cecho "Compiling Python..." $boldgreen


#tar xvfz Python-${PYTHON_VERSION}.tgz
cd Python-${PYTHON_VERSION}
./configure --prefix=/opt/python${PYTHON_VERSION} --with-threads --enable-shared
make
make altinstall

#######################################################
# check /etc/ld.so.conf.d/opt-python${PYTHON_VERSION}.conf

if [[ ! -f /etc/ld.so.conf.d/opt-python${PYTHON_VERSION}.conf ]]; then

touch /etc/ld.so.conf.d/opt-python${PYTHON_VERSION}.conf
echo "/opt/python${PYTHON_VERSION}/lib/" >> /etc/ld.so.conf.d/opt-python${PYTHON_VERSION}.conf
ldconfig

fi # check /etc/ld.so.conf.d/opt-python${PYTHON_VERSION}.conf

ln -sf /opt/python${PYTHON_VERSION}/bin/python /usr/bin/python2.7

cd $DIR_TMP

sh setuptools-0.6c11-py2.7.egg --prefix=/opt/python${PYTHON_VERSION}

cecho "/opt/python${PYTHON_VERSION}/bin/easy_install pip" $boldyellow

/opt/python${PYTHON_VERSION}/bin/easy_install pip

ln -sf /opt/python${PYTHON_VERSION}/bin/pip /usr/bin/pip
ln -sf /opt/python${PYTHON_VERSION}/bin/virtualenv /usr/bin/virtualenv

#######################################################
# check python alias setup

PYTHONALIASCHECK=`grep 'alias python=/opt/python2.7.3/bin/python' ~/.bash_profile`

if [[ -z $PYTHONALIASCHECK ]]; then

echo "alias python=/opt/python${PYTHON_VERSION}/bin/python" >> ~/.bash_profile
echo "alias python2.7=/opt/python${PYTHON_VERSION}/bin/python" >> ~/.bash_profile
echo "PATH=$PATH:/opt/python2.7/bin" >> ~/.bash_profile
source ~/.bash_profile

fi # check python alias setup

#cd /opt/python2.7.3/bin
#./easy_install ElementTree
#./easy_install Markdown
#./easy_install html5lib
#./easy_install python-openid

    echo "*************************************************"
    cecho "* Python Update installed " $boldgreen
    echo "*************************************************"
fi


}