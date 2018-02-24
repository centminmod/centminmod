#!/bin/bash
################################################
# package up the Centmin Mod required downloads
# from /svr-setup into a zip package you can
# reuse locally for new installs without having
# to redownload the entire info
################################################
DT=$(date +"%d%m%y-%H%M%S")
PKGBASE='/home'
PKGDIR="${PKGBASE}/centminmod_pkg"
PKGNAME='centminmod_pkg'

################################################
# set locale temporarily to english
# due to some non-english locale issues
export LC_ALL=en_US.UTF-8
export LANG=en_US.UTF-8
export LANGUAGE=en_US.UTF-8
export LC_CTYPE=en_US.UTF-8

shopt -s expand_aliases
for g in "" e f; do
    alias ${g}grep="LC_ALL=C ${g}grep"  # speed-up grep, egrep, fgrep
done

mkdir -p $PKGDIR

if [ ! -f /usr/bin/zip ]; then
  yum -y -q install zip
fi

cd /svr-setup
for f in $(ls /svr-setup)
 do 
	if [[ "$(stat --printf='%F' $f)" != 'directory' ]]; then 
		echo "copying $f to $PKGDIR"
		\cp -f $f $PKGDIR
	fi
done

NGXPAGESPEED_NAME=$(ls -t | grep incubator-pagespeed-ngx | head -n1)
if [[ ! -z "$NGXPAGESPEED_NAME" ]]; then
  echo
  echo "copy ngx_pagespeed + psol"
  cd /svr-setup
  \cp -a $NGXPAGESPEED_NAME $PKGDIR
fi

echo
du -sh $PKGDIR
ls -lah $PKGDIR

echo
echo "zip up package directory $PKGDIR"
cd $PKGBASE
time zip -4 -q -r ${PKGNAME}-$DT ${PKGNAME}

echo
ls -lah $PKGBASE | grep $PKGNAME

# clean up
rm -rf $PKGDIR

echo
echo "packaging completed"
echo "saved package to ${PKGBASE}/${PKGNAME}-${DT}.zip"
exit