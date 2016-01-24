# for ngx_brotli https://github.com/google/ngx_brotli
# + libbrotli https://github.com/bagder/libbrotli

scl_install() {
	# if gcc version is less than 4.7 (407) install scl collection yum repo
	if [[ "$CENTOS_SIX" = '6' ]]; then
		if [[ "$(gcc --version | head -n1 | awk '{print $3}' | cut -d . -f1,2 | sed "s|\.|0|")" -lt '407' ]]; then
			echo "install scl for newer gcc and g++ versions"
			wget http://linuxsoft.cern.ch/cern/scl/slc6-scl.repo -O /etc/yum.repos.d/slc6-scl.repo
			rpm --import http://linuxsoft.cern.ch/cern/scl/RPM-GPG-KEY-cern
			# yum -y install devtoolset-3 -q
			yum -y install devtoolset-3-gcc-c++ devtoolset-3-binutils -q
		fi
	fi # centos 6 only needed
}

libbroti_install() {
	if [[ "$NGINX_LIBBROTLI" = [yY] ]]; then

		# if devtoolset-3 doesn't exist install it otherwise check if gcc & g++ versions are less than 4.7 (407)
		if [[ "$CENTOS_SIX" = '6' ]]; then
			if [[ ! -f /opt/rh/devtoolset-3/root/usr/bin/gcc || ! -f /opt/rh/devtoolset-3/root/usr/bin/g++ ]]; then
				scl_install
    			unset CC
    			unset CXX
    			export CC="ccache /opt/rh/devtoolset-3/root/usr/bin/gcc"
		    	export CXX="ccache /opt/rh/devtoolset-3/root/usr/bin/g++"				
			elif [[ -f /opt/rh/devtoolset-3/root/usr/bin/gcc && -f /opt/rh/devtoolset-3/root/usr/bin/g++ ]] && [[ "$(gcc --version | head -n1 | awk '{print $3}' | cut -d . -f1,2 | sed "s|\.|0|")" -lt '407' ]]; then
    			unset CC
    			unset CXX
    			export CC="ccache /opt/rh/devtoolset-3/root/usr/bin/gcc"
		    	export CXX="ccache /opt/rh/devtoolset-3/root/usr/bin/g++"			
			fi
		fi # centos 6 only needed

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

		if [[ "$CENTOS_SIX" = '6' ]]; then
		if [[ ! -f /opt/rh/devtoolset-3/root/usr/bin/gcc || ! -f /opt/rh/devtoolset-3/root/usr/bin/g++ ]]; then
			scl_install
    		unset CC
    		unset CXX		
		elif [[ -f /opt/rh/devtoolset-3/root/usr/bin/gcc && -f /opt/rh/devtoolset-3/root/usr/bin/g++ ]] && [[ "$(gcc --version | head -n1 | awk '{print $3}' | cut -d . -f1,2 | sed "s|\.|0|")" -lt '407' ]]; then
    		unset CC
    		unset CXX	
		fi		
		fi # centos 6 only needed
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
cat > "/usr/local/nginx/conf/brotli_inc.conf" <<EIF
brotli on;
brotli_static on;
brotli_min_length 1000;
brotli_buffers 32 8k;
brotli_comp_level 5;
brotli_types text/plain text/css text/xml application/javascript application/x-javascript application/xml application/xml+rss application/ecmascript application/json image/svg+xml;
EIF

BROTLI_INCLUDECHECK=$(grep '\/usr\/local\/nginx\/conf\/brotli_inc.conf' /usr/local/nginx/conf/nginx.conf)

		if [[ -z "$BROTLI_INCLUDECHECK" && -f /usr/local/nginx/conf/brotli_inc.conf ]]; then
       		sed -i 's/http {/http { \ninclude \/usr\/local\/nginx\/conf\/brotli_inc.conf;/g' /usr/local/nginx/conf/nginx.conf
    	fi
    	sed -i 's|^#include \/usr\/local\/nginx\/conf\/brotli_inc.conf|include \/usr\/local\/nginx\/conf\/brotli_inc.conf;|g' /usr/local/nginx/conf/nginx.conf
    else
    	sed -i 's|^include \/usr\/local\/nginx\/conf\/brotli_inc.conf|#include \/usr\/local\/nginx\/conf\/brotli_inc.conf;|g' /usr/local/nginx/conf/nginx.conf
	fi
}