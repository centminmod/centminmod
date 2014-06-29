ccacheinstall() {
# http://ccache.samba.org/manual.html#_environment_variables
if [ "$CCACHEINSTALL" == 'y' ]; then

	# check if ccache installed first
	if [ ! -f /usr/bin/ccache ]; then

	yum${CACHESKIP} -q -y install ccache 2>/dev/null

		if [ -f /usr/bin/ccache ]; then
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

fi

}