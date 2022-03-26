check_devtoolset_php() {
  # php 7.3+ require devtoolset provided GCC 7+ to build
  # but standalone php extensions routines aren't aware
  # and don't enable GENERAL_DEVTOOLSETGCC='y'
    php-config --version | cut -d . -f1,2 | egrep -w '7.3|7.4|8.0|8.1'
    PHPSEVENTHREE_CHECKVER=$?
    echo "$PHPSEVENTHREE_CHECKVER"
    if [[ "$PHPSEVENTHREE_CHECKVER" -eq '0' ]]; then
        GENERAL_DEVTOOLSETGCC='y'
    fi
}

highergcc_cflags() {
  # if DEVTOOLSETSEVEN or DEVTOOLSETEIGHT enabled, some PHP extensions have issues with implicit-fallthrough
  if [[ "$GCCINTEL_PHP" = [yY] && "$DEVTOOLSETEIGHT" = [yY] && "$DEVTOOLSETSEVEN" = [yY] ]]; then
    DEVTOOLSETGENERAL_FALLTHROUGH='-Wimplicit-fallthrough=0'
    DEVTOOLSETGENERAL_EXTRAFLAGS=' -fcode-hoisting -Wno-cast-function-type -Wno-cast-align -Wno-implicit-function-declaration -Wno-builtin-declaration-mismatch -Wno-deprecated-declarations'
    export CFLAGS="${DEVTOOLSETGENERAL_FALLTHROUGH}${DEVTOOLSETGENERAL_EXTRAFLAGS}"
    export CXXFLAGS="$CFLAGS"
  elif [[ "$GCCINTEL_PHP" = [yY] && "$DEVTOOLSETEIGHT" = [nN] && "$DEVTOOLSETSEVEN" = [yY] ]]; then
    DEVTOOLSETGENERAL_FALLTHROUGH='-Wimplicit-fallthrough=0'
    DEVTOOLSETGENERAL_EXTRAFLAGS=' -fcode-hoisting -Wno-cast-function-type -Wno-cast-align -Wno-implicit-function-declaration -Wno-builtin-declaration-mismatch -Wno-deprecated-declarations'
    export CFLAGS="${DEVTOOLSETGENERAL_FALLTHROUGH}${DEVTOOLSETGENERAL_EXTRAFLAGS}"
    export CXXFLAGS="$CFLAGS"
  fi
}

enable_clang() {
   if [[ "$CENTOS_SIX" = 6 || "$CENTOS_SEVEN" = 7 ]] && [[ ! -f /usr/bin/clang ]]; then
      time $YUMDNFBIN -q -y install clang clang-devel${DISABLEREPO_DNF}
   fi
   if [[ "$CENTOS_EIGHT" = 8 ]] && [[ ! -f /usr/bin/clang ]]; then
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
      export CC="ccache gcc"
      export CXX="ccache g++"
      export CCACHE_CPP2=yes
   else
      export CC="gcc"
      export CXX="g++"
   fi
   CLANG_CFLAGOPT=""
   CFLAGS=""

   if [[ "$GENERAL_DEVTOOLSETGCC" = [yY] ]]; then
      # devtoolset-4 usage for ngx_pagespeed 1.10 branch which doesn't support
      # centos 6 based gcc 4.4.7
      if [[ "$CENTOS_SIX" -eq 6 || "$CENTOS_SEVEN" -eq 7 || "$CENTOS_EIGHT" -eq 8 || "$CENTOS_NINE" -eq 9 ]]; then
        if [[ "$CENTOS_EIGHT" = 8 && "$DEVTOOLSETELEVEN" = [yY] ]]; then
            if [[ ! -f /opt/rh/gcc-toolset-11/root/usr/bin/gcc || ! -f /opt/rh/gcc-toolset-11/root/usr/bin/g++ ]]; then
               yum -y install gcc-toolset-11-runtime gcc-toolset-11-toolchain gcc-toolset-11-binutils gcc-toolset-11-binutils-devel gcc-toolset-11-gcc gcc-toolset-11-gcc-c++ gcc-toolset-11-libatomic-devel gcc-toolset-11-libstdc++-devel gcc-toolset-11-ltrace gcc-toolset-11-make gcc-toolset-11-make-devel gcc-toolset-11-annobin-plugin-gcc gcc-toolset-11-annobin-annocheck gcc-toolset-11-gdb gcc-toolset-11-dwz gcc-toolset-11-elfutils gcc-toolset-11-elfutils-devel gcc-toolset-11-elfutils-libs gcc-toolset-11-strace gcc-toolset-11-gcc-gfortran gcc-toolset-11-memstomp gcc-toolset-11-elfutils-libelf gcc-toolset-11-elfutils-libelf-devel gcc-toolset-11-libasan-devel gcc-toolset-11-libgccjit-devel gcc-toolset-11-libitm-devel gcc-toolset-11-liblsan-devel gcc-toolset-11-libquadmath-devel gcc-toolset-11-libtsan-devel gcc-toolset-11-libubsan-devel gcc-toolset-11-elfutils-debuginfod-client gcc-toolset-11-elfutils-debuginfod-client-devel gcc-toolset-11-elfutils-libs gcc-toolset-11-annobin-docs gcc-toolset-11-libgccjit gcc-toolset-11-libgccjit-devel gcc-toolset-11-libgccjit-docs
            fi       
            source /opt/rh/gcc-toolset-11/enable
            GENERALDEVTOOLSET_FALLTHROUGH=' -Wimplicit-fallthrough=0'
            GENERALDEVTOOLSET_EXTRAFLAGS=' -fcode-hoisting -Wno-cast-function-type -Wno-error=cast-align -Wno-implicit-function-declaration -Wno-builtin-declaration-mismatch -Wno-deprecated-declarations -Wno-int-conversion -Wno-error=unused-result -Wno-unused-result'
        elif [[ "$CENTOS_EIGHT" = 8 && "$DEVTOOLSETTEN" = [yY] ]]; then
            if [[ ! -f /opt/rh/gcc-toolset-10/root/usr/bin/gcc || ! -f /opt/rh/gcc-toolset-10/root/usr/bin/g++ ]]; then
               yum -y install gcc-toolset-10-binutils gcc-toolset-10-gcc gcc-toolset-10-gcc-c++
            fi       
            source /opt/rh/gcc-toolset-10/enable
            GENERALDEVTOOLSET_FALLTHROUGH=' -Wimplicit-fallthrough=0'
            GENERALDEVTOOLSET_EXTRAFLAGS=' -fcode-hoisting -Wno-cast-function-type -Wno-error=cast-align -Wno-implicit-function-declaration -Wno-builtin-declaration-mismatch -Wno-deprecated-declarations -Wno-int-conversion -Wno-error=unused-result -Wno-unused-result'
        elif [[ "$CENTOS_EIGHT" = 8 && "$DEVTOOLSETNINE" = [yY] ]]; then
            if [[ ! -f /opt/rh/gcc-toolset-9/root/usr/bin/gcc || ! -f /opt/rh/gcc-toolset-9/root/usr/bin/g++ ]]; then
               yum -y install gcc-toolset-9-binutils gcc-toolset-9-gcc gcc-toolset-9-gcc-c++
            fi       
            source /opt/rh/gcc-toolset-9/enable
            GENERALDEVTOOLSET_FALLTHROUGH=' -Wimplicit-fallthrough=0'
            GENERALDEVTOOLSET_EXTRAFLAGS=' -fcode-hoisting -Wno-cast-function-type -Wno-error=cast-align -Wno-implicit-function-declaration -Wno-builtin-declaration-mismatch -Wno-deprecated-declarations -Wno-int-conversion -Wno-error=unused-result -Wno-unused-result'
        elif [[ "$CENTOS_EIGHT" = 8 && "$DEVTOOLSETEIGHT" = [yY] ]]; then
            GENERALDEVTOOLSET_FALLTHROUGH=' -Wimplicit-fallthrough=0'
            GENERALDEVTOOLSET_EXTRAFLAGS=' -fcode-hoisting -Wno-cast-function-type -Wno-error=cast-align -Wno-implicit-function-declaration -Wno-builtin-declaration-mismatch -Wno-deprecated-declarations'
        elif [[ "$CENTOS_SEVEN" = 7 && "$DEVTOOLSETELEVEN" = [yY] ]]; then
            if [[ ! -f /opt/rh/devtoolset-11/root/usr/bin/gcc || ! -f /opt/rh/devtoolset-11/root/usr/bin/g++ ]]; then
               scl_install
            fi       
            source /opt/rh/devtoolset-11/enable
            GENERALDEVTOOLSET_FALLTHROUGH=' -Wimplicit-fallthrough=0'
            GENERALDEVTOOLSET_EXTRAFLAGS=' -fcode-hoisting -Wno-cast-function-type -Wno-cast-align -Wno-implicit-function-declaration -Wno-builtin-declaration-mismatch -Wno-deprecated-declarations -Wno-int-conversion -Wno-unused-result -Wno-unused-result -Wno-vla-parameter -Wno-maybe-uninitialized -Wno-array-bounds -Wno-stringop-overread'
        elif [[ "$CENTOS_SEVEN" = 7 && "$DEVTOOLSETTEN" = [yY] ]]; then
            if [[ ! -f /opt/rh/devtoolset-10/root/usr/bin/gcc || ! -f /opt/rh/devtoolset-10/root/usr/bin/g++ ]]; then
               scl_install
            fi       
            source /opt/rh/devtoolset-10/enable
            GENERALDEVTOOLSET_FALLTHROUGH=' -Wimplicit-fallthrough=0'
            GENERALDEVTOOLSET_EXTRAFLAGS=' -fcode-hoisting -Wno-cast-function-type -Wno-cast-align -Wno-implicit-function-declaration -Wno-builtin-declaration-mismatch -Wno-deprecated-declarations -Wno-int-conversion -Wno-unused-result -Wno-unused-result'
         elif [[ "$CENTOS_SEVEN" = 7 && "$DEVTOOLSETNINE" = [yY] && -f /opt/gcc9/bin/gcc && -f /opt/gcc9/bin/g++ ]]; then
            source /opt/gcc9/enable
            GENERALDEVTOOLSET_FALLTHROUGH=' -Wimplicit-fallthrough=0'
            GENERALDEVTOOLSET_EXTRAFLAGS=' -fcode-hoisting -Wno-cast-function-type -Wno-cast-align -Wno-implicit-function-declaration -Wno-builtin-declaration-mismatch -Wno-deprecated-declarations -Wno-int-conversion -Wno-unused-result -Wno-unused-result'
         elif [[ "$CENTOS_SEVEN" = 7 && "$DEVTOOLSETNINE" = [yY] ]]; then
            if [[ ! -f /opt/rh/devtoolset-9/root/usr/bin/gcc || ! -f /opt/rh/devtoolset-9/root/usr/bin/g++ ]]; then
               scl_install
            fi       
            source /opt/rh/devtoolset-9/enable
            GENERALDEVTOOLSET_FALLTHROUGH=' -Wimplicit-fallthrough=0'
            GENERALDEVTOOLSET_EXTRAFLAGS=' -fcode-hoisting -Wno-cast-function-type -Wno-cast-align -Wno-implicit-function-declaration -Wno-builtin-declaration-mismatch -Wno-deprecated-declarations -Wno-int-conversion -Wno-unused-result -Wno-unused-result'
         elif [[ "$CENTOS_SEVEN" = 7 && "$DEVTOOLSETEIGHT" = [yY] ]]; then
            if [[ ! -f /opt/rh/devtoolset-8/root/usr/bin/gcc || ! -f /opt/rh/devtoolset-8/root/usr/bin/g++ ]]; then
               scl_install
            fi       
            source /opt/rh/devtoolset-8/enable
            GENERALDEVTOOLSET_FALLTHROUGH=' -Wimplicit-fallthrough=0'
            GENERALDEVTOOLSET_EXTRAFLAGS=' -fcode-hoisting -Wno-cast-function-type -Wno-cast-align -Wno-implicit-function-declaration -Wno-builtin-declaration-mismatch -Wno-deprecated-declarations'
         elif [[ "$CENTOS_SIX" = 6 && "$DEVTOOLSETNINE" = [yY] ]]; then
            if [[ ! -f /opt/rh/devtoolset-9/root/usr/bin/gcc || ! -f /opt/rh/devtoolset-9/root/usr/bin/g++ ]]; then
               scl_install
            fi       
            source /opt/rh/devtoolset-9/enable
            GENERALDEVTOOLSET_FALLTHROUGH=' -Wimplicit-fallthrough=0'
            GENERALDEVTOOLSET_EXTRAFLAGS=' -fcode-hoisting -Wno-cast-function-type -Wno-cast-align -Wno-implicit-function-declaration -Wno-builtin-declaration-mismatch -Wno-deprecated-declarations'
         elif [[ "$CENTOS_SIX" = 6 && "$DEVTOOLSETEIGHT" = [yY] ]]; then
            if [[ ! -f /opt/rh/devtoolset-8/root/usr/bin/gcc || ! -f /opt/rh/devtoolset-8/root/usr/bin/g++ ]]; then
               scl_install
            fi       
            source /opt/rh/devtoolset-8/enable
            GENERALDEVTOOLSET_FALLTHROUGH=' -Wimplicit-fallthrough=0'
            GENERALDEVTOOLSET_EXTRAFLAGS=' -fcode-hoisting -Wno-cast-function-type -Wno-cast-align -Wno-implicit-function-declaration -Wno-builtin-declaration-mismatch -Wno-deprecated-declarations'
         elif [[ "$DEVTOOLSETSEVEN" = [yY] ]]; then
            if [[ ! -f /opt/rh/devtoolset-7/root/usr/bin/gcc || ! -f /opt/rh/devtoolset-7/root/usr/bin/g++ ]]; then
               scl_install
            fi       
            source /opt/rh/devtoolset-7/enable
            GENERALDEVTOOLSET_FALLTHROUGH=' -Wimplicit-fallthrough=0'
            GENERALDEVTOOLSET_EXTRAFLAGS=' -fcode-hoisting -Wno-cast-function-type -Wno-cast-align -Wno-implicit-function-declaration -Wno-builtin-declaration-mismatch -Wno-deprecated-declarations'
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
      # gcc --version | tee ${CENTMINLOGDIR}/gcc_general_native.log
      # gcc -c -Q -march=native --help=target | egrep '\[enabled\]|mtune|march|mfpmath' | tee -a ${CENTMINLOGDIR}/gcc_general_native.log

      echo
      gcc -c -Q -march=native --help=target | egrep '\-mtune|\-mprefer-vector-width|\-march|\-msse|\-msse2|\-msse2avx|\-msse3|\-mavx|\-msse4|\-mssse3|\-mpclmul' | tee ${CENTMINLOGDIR}/gcc_general_native.log
      echo
      gcc -c -Q -march=x86-64 --help=target | egrep '\-mtune|\-mprefer-vector-width|\-march|\-msse|\-msse2|\-msse2avx|\-msse3|\-mavx|\-msse4|\-mssse3|\-mpclmul' | tee ${CENTMINLOGDIR}/gcc_general_x86-64.log
      echo
      diff -y --suppress-common-lines ${CENTMINLOGDIR}/gcc_general_native.log ${CENTMINLOGDIR}/gcc_general_x86-64.log | tee ${CENTMINLOGDIR}/gcc_general_diff_native-vs-x86-64.log
      echo
      egrep '\-march=|\-mtune|\-mavx|\-msse4|\-mpclmul' ${CENTMINLOGDIR}/gcc_general_diff_native-vs-x86-64.log
      # if non native target x86-64 is used 
      # pick up important native cpu instructions
      # and manually pass them to x86-64 target
      if [[ "$MARCH_TARGETNATIVE" = [yY] ]]; then
        GCC_NONNATIVEFLAGS=""
      else
        GCC_NONNATIVEFLAGS=" $(egrep '\-march=|\-mtune|\-mavx|\-msse4|\-mpclmul' ${CENTMINLOGDIR}/gcc_general_native.log | egrep -v 'march|mtune|avx512' | grep enabled | sed -e 's|\[||g' -e 's|\]||g' -e 's|enabled||g'| xargs)"
        echo
        gcc -c -Q -march=x86-64${GCC_NONNATIVEFLAGS} --help=target | egrep '\-mtune|\-mprefer-vector-width|\-march|\-msse|\-msse2|\-msse2avx|\-msse3|\-mavx|\-msse4|\-mssse3|\-mpclmul' | tee ${CENTMINLOGDIR}/gcc_general_x86-64-non-native.log
      fi
      echo
      echo "GCC_NONNATIVEFLAGS=$GCC_NONNATIVEFLAGS"
      echo

      if [[ "$(uname -m)" = 'x86_64' && "$CPUVENDOR" = 'GenuineIntel' && "$SSECHECK" = '[enabled]' ]]; then
         CCM=64
         GEN_MTUNEOPT="-m${CCM} -march=${MARCH_TARGET}${GCC_NONNATIVEFLAGS}${GENERALDEVTOOLSET_FALLTHROUGH}${GENERALDEVTOOLSET_EXTRAFLAGS}"
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