ccacheinstall() {
# http://ccache.samba.org/manual.html#_environment_variables
if [ "$CCACHEINSTALL" == 'y' ]; then
	if [ ! -f /usr/local/bin/ccache ]; then
		# install ccache 3.2.2+
		cd $DIR_TMP
		cd ccache-${CCACHE_VER}/
		make clean
		./configure
		make${MAKETHREADS}
		make install

		if [ -f /usr/local/bin/ccache ]; then
			mkdir -p /home/.ccache
			chmod 700 /home/.ccache
			echo "run_second_cpp = true" > /home/.ccache/ccache.conf
			echo "export CCACHE_DIR=\"/home/.ccache\"" >> ~/.bashrc
			echo "export CC=\"ccache gcc\"" >> ~/.bashrc
			echo "export CXX=\"ccache g++\"" >> ~/.bashrc
			#echo "export PATH=\"/usr/lib64/ccache:$PATH\"" >> ~/.bashrc
			echo "export PATH=\"/usr/$LIBDIR/ccache:$PATH\"" >> ~/.bashrc
			source ~/.bashrc
			ccache -M $CCACHESIZE
		fi		
	fi

	# check if ccache YUM installed version and remove outdated 3.1.9
	if [ ! -f /usr/bin/ccache ]; then
		yum -y remove ccache	
	fi

fi

}