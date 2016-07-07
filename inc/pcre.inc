source_pcreinstall() {
  if [[ "$PCRE_SOURCEINSTALL" = [yY] ]]; then
    echo "*************************************************"
    cecho "* Source Install PCRE" $boldgreen
    echo "*************************************************"
  
    cd $DIR_TMP
    echo "Compiling PCRE..."
    cd pcre-${PCRE_VERSION}
    make clean
    ./configure --enable-pcre16 --enable-pcre32 --enable-pcregrep-libz --enable-pcregrep-libbz2 --enable-pcretest-libreadline
    make$MAKETHREADS
    make install
  fi
}