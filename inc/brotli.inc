# for ngx_brotli https://github.com/google/ngx_brotli
# + libbrotli https://github.com/bagder/libbrotli

libbroti_install() {
	if [[ "$NGINX_LIBBROTLI" = [yY] ]]; then

    unset CC
    unset CXX
    export CC="ccache /usr/bin/gcc"
    export CXX="ccache /usr/bin/g++ -std=c++0x"

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