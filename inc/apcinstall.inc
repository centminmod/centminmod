apcdevfix() {
    cd $DIR_TMP
  rm -rf APC-3.1.13*
  wget -c https://centminmod.com/centminmodparts/apc/php550/APC-3.1.13.tgz
  tar xfz APC-3.1.13.tgz
  cd APC-3.1.13
}

function funct_apcsourceinstall {

    echo "*************************************************"
    cecho "* Installing Alternative PHP Cache" $boldgreen
    echo "*************************************************"

        if [[ "$CLANG_APC" = [yY] ]]; then
            if [[ "$CENTOS_SIX" = '6' && ! -f /usr/bin/clang ]] || [[ "$CENTOS_SEVEN" = '7' && ! -f /bin/clang ]]; then
                time $YUMDNFBIN -q -y install clang clang-devel${DISABLEREPO_DNF}
            fi
            export CC="ccache /usr/bin/clang -ferror-limit=0"
            export CXX="ccache /usr/bin/clang++ -ferror-limit=0"
            export CCACHE_CPP2=yes
            CLANG_CFLAGOPT='-Wno-deprecated-declarations -Wno-unused-parameter -Wno-unused-const-variable -Wno-conditional-uninitialized -Wno-mismatched-tags -Wno-sometimes-uninitialized -Wno-parentheses-equality -Wno-tautological-compare -Wno-self-assign -Wno-deprecated-register -Wno-deprecated -Wno-invalid-source-encoding -Wno-pointer-sign -Wno-parentheses -Wno-enum-conversion -Wno-implicit-function-declaration -Wno-incompatible-library-redeclaration -Wno-format -Wno-incompatible-pointer-types -Wno-undefined-inline -Wno-unused-function -Wno-int-conversion -Wno-implicit-function-declaration -Wno-incompatible-library-redeclaration -Wno-format -Wno-non-literal-null-conversion'
            export CFLAGS="$CLANG_CFLAGOPT"
        else
            export CC="ccache /usr/bin/gcc"
            export CXX="ccache /usr/bin/g++"
            export CCACHE_CPP2=yes
            CLANG_CFLAGOPT=""
            CFLAGS=""
        fi           

    cd $DIR_TMP

PHPCURRENTVER=$(php -v | awk -F " " '{print $2}' | head -n1 | cut -d . -f1,2)

#download apc tarball
if [ -z "$APCINSTALL" ]; then
apctarball
fi

    export PHP_AUTOCONF=/usr/bin/autoconf
    export PHP_AUTOHEADER=/usr/bin/autoheader

if [[ "$PHPCURRENTVER" = '5.5' ]]; then
    apcdevfix
elif [[ "$PHPCURRENTVER" = 5.[234] ]]; then
    #tar xvzf APC-${APCCACHE_VERSION}.tgz
    cd APC-${APCCACHE_VERSION}
elif [[ "$PHPCURRENTVER" > 5.5 ]]; then
    echo "Your current PHP $PHPCURRENTVER version is not supported. Use Zend Opcache instead"
    if [[ "$INITIALINSTALL" != [yY] ]]; then
        exit
    fi
fi

/usr/local/bin/phpize

if [ "$APCCACHE_VERSION" == '3.1.10' ]; then
sed -i 's/PHP_APC_VERSION \"3.1.9\"/PHP_APC_VERSION \"3.1.10\"/' php_apc.h
fi

./configure --with-php-config=/usr/local/bin/php-config
make${MAKETHREADS}
make install

        if [[ "$CLANG_APC" = [yY] ]]; then
            unset CC
            unset CXX
            unset CFLAGS
            #unset CCACHE_CPP2
            export CC="ccache /usr/bin/gcc"
            export CXX="ccache /usr/bin/g++"
            CLANG_CFLAGOPT=""
            CFLAGS=""            
        fi   

if [ -f ${CONFIGSCANDIR}/xcache.ini ]; then
  rm -rf ${CONFIGSCANDIR}/xcache.ini
fi

if [ -f ${CONFIGSCANDIR}/zendopcache.ini ]; then
  rm -rf ${CONFIGSCANDIR}/zendopcache.ini
fi

# Randomize myapc.php filename
N=$(od -vAn -N8 -tx < /dev/urandom | sed -e 's/\s//g')
cp apc.php /usr/local/nginx/html/${N}_myapc.php

if [ -f /usr/local/nginx/html/myapc.php ]; then
mv /usr/local/nginx/html/myapc.php /usr/local/nginx/html/${N}_myapc.php
fi

chown nginx:nginx /usr/local/nginx/html/${N}_myapc.php

#######################################################
# check if apc.so exists in php.ini

APCSOCHECK=`grep 'extension=apc.so' /usr/local/lib/php.ini`
APCSOCHECKB=$(grep 'extension=apc.so' ${CONFIGSCANDIR}/apc.ini)

if [[ -z $APCSOCHECK || -z $APCSOCHECKB ]]; then

echo

#read -ep "Does this server have less than <=2048MB of memory installed ? [y/n]: " lessapcmem

#echo
#echo

if [[ "$lessapcmem" = [yY] ]]; then

echo $lessapcmem

echo -e "\nCopying apc-min.ini > ${CONFIGSCANDIR}/apc.ini\n"
#cat $CUR_DIR/config/apc/apc-min.ini >> /usr/local/lib/php.ini
cat $CUR_DIR/config/apc/apc-min.ini > ${CONFIGSCANDIR}/apc.ini

else

echo $lessapcmem

echo -e "\nCopying apc.ini > ${CONFIGSCANDIR}/apc.ini\n"
#cat $CUR_DIR/config/apc/apc.ini >> /usr/local/lib/php.ini
cat $CUR_DIR/config/apc/apc-min.ini > ${CONFIGSCANDIR}/apc.ini

fi

fi # check if apc.so exists in php.ini

    cmservice php-fpm restart
    echo "*************************************************"
    cecho "* Alternative PHP Cache installed" $boldgreen
    echo "*************************************************"

funct_igbinaryinstall

echo
echo

echo ""
cecho "Important instructions next to protect your apc.php page" $boldgreen
  if [ "$UNATTENDED" == 'n' ]; then
  sleep 20
  else
  sleep 2
  fi #unattended

apcprotect

echo
echo

}

function funct_installapc {
    if [[ "$PHP_INSTALL" = [yY] ]]; then
funct_apcsourceinstall

if [[ "$ENABLE_MENU" != [yY] ]]; then

ASK "Do would you like to run script cleanup (Highly recommended) ? [y/n] "
if [[ "$key" = [yY] ]];
then
    rm -rf $DIR_TMP
    echo "Temporary files/folders removed"
fi

ASK "Do you want to delete this script ? [y/n] "
if [[ "$key" = [yY] ]];
then
    echo "*************************************************"
    cecho "* Deleting Centmin script... " $boldgreen
    echo "*************************************************"
    echo "Removing..."

rm -f $0

    echo "*************************************************"
    cecho "* Centmin script deleted" $boldgreen
    echo "*************************************************"
fi

fi
    fi # PHP_INSTALL=y
}