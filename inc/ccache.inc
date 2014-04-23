ccacheinstall() {

if [ "$CCACHEINSTALL" == 'y' ]; then

# check if ccache installed first
if [ ! -f /usr/bin/ccache ]; then

yum -q -y install ccache
mkdir -p /home/.ccache
chmod 700 /home/.ccache
echo "export CCACHE_DIR=\"/home/.ccache\"" >> ~/.bashrc
echo "export CC=\"ccache gcc\"" >> ~/.bashrc
echo "export CXX=\"ccache g++\"" >> ~/.bashrc
#echo "export PATH=\"/usr/lib64/ccache:$PATH\"" >> ~/.bashrc
echo "export PATH=\"/usr/$LIBDIR/ccache:$PATH\"" >> ~/.bashrc
source ~/.bashrc
ccache -M $CCACHESIZE

fi

fi

}