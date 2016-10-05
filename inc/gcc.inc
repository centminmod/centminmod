enable_devtoolset() {
# devtoolset-3 usage for ngx_pagespeed 1.10 branch which doesn't support
# centos 6 based gcc 4.4.7
if [[ "$CENTOS_SIX" = '6' && "$GENERAL_DEVTOOLSETGCC" = [yY] ]]; then
  if [[ ! -f /opt/rh/devtoolset-3/root/usr/bin/gcc || ! -f /opt/rh/devtoolset-3/root/usr/bin/g++ ]]; then
    scl_install
    if [[ "$INITIALINSTALL" != [yY] ]]; then
      source /opt/rh/devtoolset-3/enable
      which gcc
      which g++
      unset CC
      unset CXX
      export CC="ccache gcc"
      export CXX="ccache g++"
    else
      source /opt/rh/devtoolset-3/enable
      which gcc
      which g++
      unset CC
      unset CXX
      export CC="gcc"
      export CXX="g++"
    fi
  elif [[ -f /opt/rh/devtoolset-3/root/usr/bin/gcc && -f /opt/rh/devtoolset-3/root/usr/bin/g++ ]]; then
    # update devtoolset-3 repo
    if [[ "$CENTOS_SIX" = '6' && -f /etc/yum.repos.d/slc6-scl.repo ]]; then
      yum -y remove $(yum -q list installed --disableplugin=priorities | awk '/slc6-scl/ {print $1}')
      rm -rf /etc/yum.repos.d/slc6-scl.repo
      yum clean all
      #yum install centos-release-scl-rh --disableplugin=fastmirror
      wget -O /etc/yum.repos.d/rhscl-devtoolset-3-epel-6.repo https://copr.fedorainfracloud.org/coprs/rhscl/devtoolset-3/repo/epel-6/rhscl-devtoolset-3-epel-6.repo
      yum -y install devtoolset-3-gcc devtoolset-3-gcc-c++ devtoolset-3-binutils
      echo
      /opt/rh/devtoolset-3/root/usr/bin/gcc --version
      /opt/rh/devtoolset-3/root/usr/bin/g++ --version
    fi
    if [[ "$INITIALINSTALL" != [yY] ]]; then
      source /opt/rh/devtoolset-3/enable
      which gcc
      which g++
      unset CC
      unset CXX
      export CC="ccache gcc"
      export CXX="ccache g++"
    else
      source /opt/rh/devtoolset-3/enable
      which gcc
      which g++
      unset CC
      unset CXX
      export CC="gcc"
      export CXX="g++"
    fi
  fi
fi

# devtoolset-3 usage of GCC 4.9 even for CentOS7
if [[ "$CENTOS_SEVEN" = '7' && "$GENERAL_DEVTOOLSETGCC" = [yY] ]]; then
  if [[ ! -f /opt/rh/devtoolset-3/root/usr/bin/gcc || ! -f /opt/rh/devtoolset-3/root/usr/bin/g++ ]]; then
    scl_install
    if [[ "$INITIALINSTALL" != [yY] ]]; then
      source /opt/rh/devtoolset-3/enable
      which gcc
      which g++
      unset CC
      unset CXX
      export CC="ccache gcc"
      export CXX="ccache g++"
    else
      source /opt/rh/devtoolset-3/enable
      which gcc
      which g++
      unset CC
      unset CXX
      export CC="gcc"
      export CXX="g++"
    fi
  elif [[ -f /opt/rh/devtoolset-3/root/usr/bin/gcc && -f /opt/rh/devtoolset-3/root/usr/bin/g++ ]]; then
    if [[ "$INITIALINSTALL" != [yY] ]]; then
      source /opt/rh/devtoolset-3/enable
      which gcc
      which g++
      unset CC
      unset CXX
      export CC="ccache gcc"
      export CXX="ccache g++"
    else
      source /opt/rh/devtoolset-3/enable
      which gcc
      which g++
      unset CC
      unset CXX
      export CC="gcc"
      export CXX="g++"
    fi
  fi
fi

if [[ "$GENERAL_DEVTOOLSETGCC" = [yY] ]]; then
  # intel specific
  CPUVENDOR=$(cat /proc/cpuinfo | awk '/vendor_id/ {print $3}' | sort -u | head -n1)
  SSECHECK=$(gcc -c -Q -march=native --help=target | awk '/  -msse/ {print $2}' | head -n1)
  gcc --version | tee ${CENTMINLOGDIR}/gcc_general_native.log
  gcc -c -Q -march=native --help=target | egrep '\[enabled\]|mtune|march|mfpmath' | tee -a ${CENTMINLOGDIR}/gcc_general_native.log
  if [[ "$(uname -m)" = 'x86_64' && "$CPUVENDOR" = 'GenuineIntel' && "$SSECHECK" = '[enabled]' ]]; then
      CCM=64
      GEN_MTUNEOPT="-m${CCM} -march=native"
      # if only 1 cpu thread use -O2 to keep compile times sane
      if [[ "$CPUS" = '1' ]]; then
        export CFLAGS="-O2 $GEN_MTUNEOPT -pipe"
      else
        export CFLAGS="-O3 $GEN_MTUNEOPT -pipe"
      fi
      export CXXFLAGS="$CFLAGS"
  fi
fi
}

disable_devtoolset() {
  if [[ "$GENERAL_DEVTOOLSETGCC" = [yY] ]]; then
    if [[ "$(uname -m)" == 'x86_64' && $(grep Intel /proc/cpuinfo) ]]; then
        echo "$CFLAGS"
        echo "$CXXFLAGS"
        unset CFLAGS
        unset CXXFLAGS
    elif [[ "$(uname -m)" != 'x86_64' && $(grep Intel /proc/cpuinfo) ]]; then
        echo "$CFLAGS"
        echo "$CXXFLAGS"
        unset CFLAGS
        unset CXXFLAGS
    fi
  fi
}

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