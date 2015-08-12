patchnginx() {
	if [[ "$NGINX_HTTP2" = [yY] ]]; then
	# http/2 patch
	echo
	cecho "patching nginx for http/2 support" $boldyellow
	echo
	cecho "wget http://nginx.org/patches/http2/patch.http2.txt" $boldyellow
	wget http://nginx.org/patches/http2/patch.http2.txt

	cecho "patch -p1 < patch.http2.txt" $boldyellow
	patch -p1 < patch.http2.txt

	echo
	cecho "nginx patched for http/2" $boldyellow
	echo
	fi
}