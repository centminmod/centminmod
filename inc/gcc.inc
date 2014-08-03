set_intelflags() {
	if [[ "$INTELOPT" = [yY] ]]; then
    	if [[ "$(uname -m)" == 'x86_64' && $(grep Intel /proc/cpuinfo) ]]; then
        	CFLAGS='-O2 -m64 -march=native -pipe -g -mmmx -msse3'
        	CXXFLAGS='-O2 -m64 -march=native -pipe -g -mmmx -msse3' 
        	export CFLAGS
        	export CXXFLAGS
    	elif [[ "$(uname -m)" != 'x86_64' && $(grep Intel /proc/cpuinfo) ]]; then
        	CFLAGS='-O2 -m32 -march=native -pipe -g -mmmx -msse3'
        	CXXFLAGS='-O2 -m32 -march=native -pipe -g -mmmx -msse3' 
        	export CFLAGS
        	export CXXFLAGS
    	fi
	fi
}

unset_intelflags() {
	if [[ "$INTELOPT" = [yY] ]]; then
    	if [[ "$(uname -m)" == 'x86_64' && $(grep Intel /proc/cpuinfo) ]]; then
        	unset CFLAGS
        	unset CXXFLAGS
    	elif [[ "$(uname -m)" != 'x86_64' && $(grep Intel /proc/cpuinfo) ]]; then
        	unset CFLAGS
        	unset CXXFLAGS
    	fi
	fi
}


# function currently disabled not ready for use
checkgcc() {

if [[ "$INTELOPT" = [yY] ]]; then
	NGINX_GCCOPT='y'
else
	NGINX_GCCOPT='n'
fi

if [[ "$NGINX_GCCOPT" = [yY] ]]; then

if [[ "$(uname -m)" = 'x86_64' ]]; then
  CCM=64
else
  CCM=32
fi

# check native flags for march
# gcc -march=native -Q --help=target

# check flags supported by cpu
cat /proc/cpuinfo | grep flags | sort -u

CPUVENDOR=$(cat /proc/cpuinfo | awk '/vendor_id/ {print $3}' | sort -u)

if [[ "$CPUVENDOR" != 'GenuineIntel' ]]; then
  CPUCCOPT="--with-cc-opt="-O2 -g -pipe -Wall -Wp,-D_FORTIFY_SOURCE=2 -fexceptions -fstack-protector-strong --param=ssp-buffer-size=4 -m${CCM} -mtune=generic""
else
  CPUCCOPT="--with-cc-opt="-O2 -g -pipe -Wall -Wp,-D_FORTIFY_SOURCE=2 -fexceptions -fstack-protector-strong --param=ssp-buffer-size=4 -m${CCM} -mtune=native""
fi

echo $CPUVENDOR
echo $CPUCCOPT

fi

}