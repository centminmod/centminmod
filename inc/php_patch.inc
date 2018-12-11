php_patches() {
  {
  if [[ "$PHP_PATCH" = [yY] ]]; then
    if [[ "$PHPMUVER" = 5.3 ]]; then
      # patch PHP 5.3 source tarballs
      # https://bugs.php.net/bug.php?id=48795
      # https://community.centminmod.com/posts/63882/
      # pushd ../
      if [ ! -f php53.patch ]; then
        cp -a $CUR_DIR/patches/php/php53.patch php53.patch
        patch < php53.patch
      fi
      # popd
    fi
    if [[ "$PHPVER_ID" = '70213' || "$PHPVER_ID" = '70125' ]]; then
      # patch PHP 7.2.13 and 7.1.25 missing CVE patch
      # https://bugs.php.net/bug.php?id=77020
      if [ ! -f ext/imap/77020fix.patch ]; then
        echo
        echo "patching PHP for CVE-2018-19935"
        echo "https://bugs.php.net/bug.php?id=77020"
        echo
        pushd ext/imap
        cp -a $CUR_DIR/patches/php/77020fix.patch 77020fix.patch
        dos2unix 77020fix.patch
        patch --ignore-whitespace < 77020fix.patch
        popd
      fi
    fi
    if [[ "$PHPVER_ID" = '70300' ]]; then
      # patch PHP 7.3.0 missing CVE patch
      # https://bugs.php.net/bug.php?id=77275
      if [ ! -f 77275fix.patch ]; then
        echo
        echo "patching PHP Zend Opcache Bug"
        echo "https://bugs.php.net/bug.php?id=77275"
        echo
        cp -a $CUR_DIR/patches/php/77275fix.patch 77275fix.patch
        dos2unix 77275fix.patch
        patch --ignore-whitespace -p1 < 77275fix.patch
      fi
    fi
  fi
} 2>&1 | tee -a "${CENTMINLOGDIR}/patch_php_${DT}.log"
}