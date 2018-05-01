enable_clang() {
   if [[ "$CENTOS_SIX" = 6 || "$CENTOS_SEVEN" = 7 ]] && [[ ! -f /usr/bin/clang ]]; then
      time $YUMDNFBIN -q -y install clang clang-devel${DISABLEREPO_DNF}
   fi
   if [[ "$INITIALINSTALL" != [yY] ]]; then             
      export CC="ccache /usr/bin/clang -ferror-limit=0"
      export CXX="ccache /usr/bin/clang++ -ferror-limit=0"
      export CCACHE_CPP2=yes
   else
      export CC="/usr/bin/clang -ferror-limit=0"
      export CXX="/usr/bin/clang++ -ferror-limit=0"
   fi
   CLANG_CFLAGOPT='-Wno-deprecated-declarations -Wno-unused-parameter -Wno-unused-const-variable -Wno-conditional-uninitialized -Wno-mismatched-tags -Wno-sometimes-uninitialized -Wno-parentheses-equality -Wno-tautological-compare -Wno-self-assign -Wno-deprecated-register -Wno-deprecated -Wno-invalid-source-encoding -Wno-pointer-sign -Wno-parentheses -Wno-enum-conversion -Wno-implicit-function-declaration -Wno-incompatible-library-redeclaration -Wno-format -Wno-incompatible-pointer-types -Wno-undefined-inline -Wno-unused-function -Wno-int-conversion -Wno-implicit-function-declaration -Wno-incompatible-library-redeclaration -Wno-format -Wno-non-literal-null-conversion'
   export CFLAGS="$CLANG_CFLAGOPT"
}


enable_devtoolset() {
   # ccache compiler has some initial overhead for compiles but speeds up subsequent
   # recompiles. however on initial install ccache has no benefits, so for initial
   # centmin mod install disabling ccache will in theory speed up first time installs
   if [[ "$INITIALINSTALL" != [yY] ]]; then            
      export CC="ccache /usr/bin/gcc"
      export CXX="ccache /usr/bin/g++"
      export CCACHE_CPP2=yes
   else
      export CC="/usr/bin/gcc"
      export CXX="/usr/bin/g++"
   fi
   CLANG_CFLAGOPT=""
   CFLAGS=""

   if [[ "$GENERAL_DEVTOOLSETGCC" = [yY] ]]; then
      # devtoolset-4 usage for ngx_pagespeed 1.10 branch which doesn't support
      # centos 6 based gcc 4.4.7
      if [[ "$CENTOS_SIX" = 6 || "$CENTOS_SEVEN" = 7 ]]; then
         if [[ "$DEVTOOLSETSEVEN" = [yY] ]]; then
            if [[ ! -f /opt/rh/devtoolset-7/root/usr/bin/gcc || ! -f /opt/rh/devtoolset-7/root/usr/bin/g++ ]]; then
               scl_install
            fi       
            source /opt/rh/devtoolset-7/enable
            GENERALDEVTOOLSETSEVEN_FALLTHROUGH=' -Wimplicit-fallthrough=0'
            GENERALDEVTOOLSETSEVEN_EXTRAFLAGS=' -fcode-hoisting'
         elif [[ "$DEVTOOLSETSIX" = [yY] ]]; then
            if [[ ! -f /opt/rh/devtoolset-6/root/usr/bin/gcc || ! -f /opt/rh/devtoolset-6/root/usr/bin/g++ ]]; then
               scl_install
            fi       
            source /opt/rh/devtoolset-6/enable
         else
            if [[ ! -f /opt/rh/devtoolset-4/root/usr/bin/gcc || ! -f /opt/rh/devtoolset-4/root/usr/bin/g++ ]]; then
               scl_install
            fi
            source /opt/rh/devtoolset-4/enable
         fi
         which gcc
         which g++
         unset CC
         unset CXX
         if [[ "$INITIALINSTALL" != [yY] ]]; then
            export CC="ccache gcc"
            export CXX="ccache g++"
         else
            export CC="gcc"
            export CXX="g++"
         fi
      fi

      # intel specific
      CPUVENDOR=$(cat /proc/cpuinfo | awk '/vendor_id/ {print $3}' | sort -u | head -n1)
      SSECHECK=$(gcc -c -Q -march=native --help=target | awk '/  -msse/ {print $2}' | head -n1)
      gcc --version | tee ${CENTMINLOGDIR}/gcc_general_native.log
      gcc -c -Q -march=native --help=target | egrep '\[enabled\]|mtune|march|mfpmath' | tee -a ${CENTMINLOGDIR}/gcc_general_native.log
      if [[ "$(uname -m)" = 'x86_64' && "$CPUVENDOR" = 'GenuineIntel' && "$SSECHECK" = '[enabled]' ]]; then
         CCM=64
         GEN_MTUNEOPT="-m${CCM} -march=${MARCH_TARGET}${GENERALDEVTOOLSETSEVEN_FALLTHROUGH}${GENERALDEVTOOLSETSEVEN_EXTRAFLAGS}"
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
          CFLAGS='-O2 -m64 -march=${MARCH_TARGET} -pipe -g'
          CXXFLAGS='-O2 -m64 -march=${MARCH_TARGET} -pipe -g' 
          export CFLAGS
          export CXXFLAGS
      elif [[ "$(uname -m)" != 'x86_64' && $(grep Intel /proc/cpuinfo) ]]; then
          CFLAGS='-O2 -m32 -march=native -pipe -g'
          CXXFLAGS='-O2 -m32 -march=native -pipe -g' 
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