#!/bin/bash
################################################
# package up the Centmin Mod required downloads
# from /svr-setup into a zip package you can
# reuse locally for new installs without having
# to redownload the entire info
################################################
DT=`date +"%d%m%y-%H%M%S"`
PKGBASE='/home'
PKGDIR="${PKGBASE}/centminmod_pkg"
PKGNAME='centminmod_pkg'

################################################
mkdir -p $PKGDIR

cd /svr-setup
for f in $(ls /svr-setup)
 do 
	if [[ "$(stat --printf='%F' $f)" != 'directory' ]]; then 
		echo "copying $f to $PKGDIR"
		\cp -f $f $PKGDIR
	fi
done

echo
echo "copy ngx_pagespeed + psol"
cd /svr-setup
\cp -a $(ls -t | grep ngx_pagespeed-release | head -n1) $PKGDIR

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