# for ngx_brotli https://github.com/google/ngx_brotli
# + libbrotli https://github.com/bagder/libbrotli

scl_install() {
	# if gcc version is less than 4.7 (407) install scl collection yum repo
	if [[ "$(gcc --version | head -n1 | awk '{print $3}' | cut -d . -f1,2 | sed "s|\.|0|")" -lt '407' ]]; then
		echo "install scl for newer gcc and g++ versions"
		wget http://linuxsoft.cern.ch/cern/scl/slc6-scl.repo -O /etc/yum.repos.d/slc6-scl.repo
		rpm --import http://linuxsoft.cern.ch/cern/centos/7/os/x86_64/RPM-GPG-KEY-cern
		yum -y install devtoolset-3 -q
	fi
}

libbroti_install() {
	if [[ "$NGINX_LIBBROTLI" = [yY] ]]; then

		if [[ ! -f /opt/rh/devtoolset-3/root/usr/bin/gcc || ! -f /opt/rh/devtoolset-3/root/usr/bin/g++ ]]; then
			scl_install
		elif [[ -f /opt/rh/devtoolset-3/root/usr/bin/gcc && -f /opt/rh/devtoolset-3/root/usr/bin/g++ ]] && [[ "$(gcc --version | head -n1 | awk '{print $3}' | cut -d . -f1,2 | sed "s|\.|0|")" -lt '407' ]]; then
    		unset CC
    		unset CXX
    		export CC="ccache /opt/rh/devtoolset-3/root/usr/bin/gcc"
		    export CXX="ccache /opt/rh/devtoolset-3/root/usr/bin/g++"			
		fi

		echo "install libbrotli"
		cd $DIR_TMP
		git clone https://github.com/bagder/libbrotli
		cd libbrotli
		if [[ "$INITIALINSTALL" != [yY] ]]; then
    		make clean
    		git stash
    		git pull
		fi
		./autogen.sh
		./configure
		make${MAKETHREADS}
		make install
		echo

	unset CC
    unset CXX
	fi
}

ngxbrotli_download() {
	if [[ "$NGINX_LIBBROTLI" = [yY] ]]; then
		echo "download ngx_brotli module"
		cd $DIR_TMP
		git clone https://github.com/google/ngx_brotli.git
		if [[ "$INITIALINSTALL" != [yY] ]]; then
			cd ngx_brotli
			git stash
			git pull
		fi
		echo
	fi
}