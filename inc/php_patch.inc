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
    if [[ "$PHPMUVER" = '5.6' || "$PHPMVER" = '5.6' ]]; then
      # backport security bug patch from PHP 7.1.27 to 
      # PHP 5.6 bug #77540 https://bugs.php.net/bug.php?id=77540
      if [ ! -f php5640-77540.patch ]; then
        echo
        echo "patching PHP 5.6 for bug #77540"
        echo "https://bugs.php.net/bug.php?id=77540"
        echo
        cp -a $CUR_DIR/patches/php/php5640-77540.patch php5640-77540.patch
        dos2unix php5640-77540.patch
        patch -p1 < php5640-77540.patch
      fi
      # PHP 5.6 bug #77563 https://bugs.php.net/bug.php?id=77563
      if [ ! -f php5640-77563.patch ]; then
        echo
        echo "patching PHP 5.6 for bug #77563"
        echo "https://bugs.php.net/bug.php?id=77563"
        echo
        cp -a $CUR_DIR/patches/php/php5640-77563.patch php5640-77563.patch
        dos2unix php5640-77563.patch
        patch -p1 < php5640-77563.patch
      fi
      # PHP 5.6 bug #77630 https://bugs.php.net/bug.php?id=77630
      if [ ! -f php5640-77630.patch ]; then
        echo
        echo "patching PHP 5.6 for bug #77630"
        echo "https://bugs.php.net/bug.php?id=77630"
        echo
        cp -a $CUR_DIR/patches/php/php5640-77630.patch php5640-77630.patch
        dos2unix php5640-77630.patch
        patch -p1 < php5640-77630.patch
      fi
      # PHP 5.6 bug #76846 https://bugs.php.net/bug.php?id=76846
      if [ ! -f php5640-76846.patch ]; then
        echo
        echo "patching PHP 5.6 for bug #76846"
        echo "https://bugs.php.net/bug.php?id=76846"
        echo
        cp -a $CUR_DIR/patches/php/php5640-76846.patch php5640-76846.patch
        dos2unix php5640-76846.patch
        patch -p1 < php5640-76846.patch
      fi
      # PHP 5.6 bug #77753 https://bugs.php.net/bug.php?id=77753
      if [ ! -f php5640-77753.patch ]; then
        echo
        echo "patching PHP 5.6 for bug #77753"
        echo "https://bugs.php.net/bug.php?id=77753"
        echo
        cp -a $CUR_DIR/patches/php/php5640-77753.patch php5640-77753.patch
        dos2unix php5640-77753.patch
        patch -p1 < php5640-77753.patch
      fi
      # PHP 5.6 bug #77831 https://bugs.php.net/bug.php?id=77831
      if [ ! -f php5640-77831.patch ]; then
        echo
        echo "patching PHP 5.6 for bug #77831"
        echo "https://bugs.php.net/bug.php?id=77831"
        echo
        cp -a $CUR_DIR/patches/php/php5640-77831.patch php5640-77831.patch
        dos2unix php5640-77831.patch
        patch -p1 < php5640-77831.patch
      fi
      # PHP 5.6 Backported from 7.1.28 added sqlite3.defensive INI directive
      if [ ! -f php5640-sqlite3-defensive.patch ]; then
        echo
        echo "patching PHP 5.6 for php5640-sqlite3-defensive patch"
        echo
        cp -a $CUR_DIR/patches/php/php5640-sqlite3-defensive.patch php5640-sqlite3-defensive.patch
        dos2unix php5640-sqlite3-defensive.patch
        patch -p1 < php5640-sqlite3-defensive.patch
      fi
      # PHP 5.6 bug #77950 https://bugs.php.net/bug.php?id=77950
      if [ ! -f php5640-77950.patch ]; then
        echo
        echo "patching PHP 5.6 for bug #77950"
        echo "https://bugs.php.net/bug.php?id=77950"
        echo
        cp -a $CUR_DIR/patches/php/php5640-77950.patch php5640-77950.patch
        dos2unix php5640-77950.patch
        patch -p1 < php5640-77950.patch
      fi
      # PHP 5.6 bug #77967 https://bugs.php.net/bug.php?id=77967
      if [ ! -f php5640-77967.patch ]; then
        echo
        echo "patching PHP 5.6 for bug #77967"
        echo "https://bugs.php.net/bug.php?id=77967"
        echo
        cp -a $CUR_DIR/patches/php/php5640-77967.patch php5640-77967.patch
        dos2unix php5640-77967.patch
        patch -p1 < php5640-77967.patch
      fi
      # PHP 5.6 bug #77988 https://bugs.php.net/bug.php?id=77988
      if [ ! -f php5640-77988.patch ]; then
        echo
        echo "patching PHP 5.6 for bug #77988"
        echo "https://bugs.php.net/bug.php?id=77988"
        echo
        cp -a $CUR_DIR/patches/php/php5640-77988.patch php5640-77988.patch
        dos2unix php5640-77988.patch
        patch -p1 < php5640-77988.patch
      fi
      # PHP 5.6 bug #78069 https://bugs.php.net/bug.php?id=78069
      if [ ! -f php5640-78069.patch ]; then
        echo
        echo "patching PHP 5.6 for bug #78069"
        echo "https://bugs.php.net/bug.php?id=78069"
        echo
        cp -a $CUR_DIR/patches/php/php5640-78069.patch php5640-78069.patch
        dos2unix php5640-78069.patch
        patch -p1 < php5640-78069.patch
      fi
      # PHP 5.6 bug #77919 https://bugs.php.net/bug.php?id=77919
      if [ ! -f php5640-77919.patch ]; then
        echo
        echo "patching PHP 5.6 for bug #77919"
        echo "https://bugs.php.net/bug.php?id=77919"
        echo
        cp -a $CUR_DIR/patches/php/php5640-77919.patch php5640-77919.patch
        dos2unix php5640-77919.patch
        patch -p1 < php5640-77919.patch
      fi
      # PHP 5.6 bug #78222 https://bugs.php.net/bug.php?id=78222
      if [ ! -f php5640-78222.patch ]; then
        echo
        echo "patching PHP 5.6 for bug #78222"
        echo "https://bugs.php.net/bug.php?id=78222"
        echo
        cp -a $CUR_DIR/patches/php/php5640-78222.patch php5640-78222.patch
        dos2unix php5640-78222.patch
        patch -p1 < php5640-78222.patch
      fi
      # PHP 5.6 bug #78256 https://bugs.php.net/bug.php?id=78256
      if [ ! -f php5640-78256.patch ]; then
        echo
        echo "patching PHP 5.6 for bug #78256"
        echo "https://bugs.php.net/bug.php?id=78256"
        echo
        cp -a $CUR_DIR/patches/php/php5640-78256.patch php5640-78256.patch
        dos2unix php5640-78256.patch
        patch -p1 < php5640-78256.patch
      fi
      # PHP 5.6 bug #75457 https://bugs.php.net/bug.php?id=75457
      if [ ! -f php5640-75457.patch ]; then
        echo
        echo "patching PHP 5.6 for bug #75457"
        echo "https://bugs.php.net/bug.php?id=75457"
        echo
        cp -a $CUR_DIR/patches/php/php5640-75457.patch php5640-75457.patch
        dos2unix php5640-75457.patch
        patch -p1 < php5640-75457.patch
      fi
      # PHP 5.6 bug #78380 https://bugs.php.net/bug.php?id=78380
      if [ ! -f php5640-78380.patch ]; then
        echo
        echo "patching PHP 5.6 for bug #78380"
        echo "https://bugs.php.net/bug.php?id=78380"
        echo
        cp -a $CUR_DIR/patches/php/php5640-78380.patch php5640-78380.patch
        dos2unix php5640-78380.patch
        patch -p1 < php5640-78380.patch
      fi
      # PHP 5.6 bug #78599 https://bugs.php.net/bug.php?id=78599
      if [ ! -f php5640-78599.patch ]; then
        echo
        echo "patching PHP 5.6 for bug #78599"
        echo "https://bugs.php.net/bug.php?id=78599"
        echo
        cp -a $CUR_DIR/patches/php/php5640-78599.patch php5640-78599.patch
        dos2unix php5640-78599.patch
        patch -p1 < php5640-78599.patch
      fi
      # PHP 5.6 bug #78862 https://bugs.php.net/bug.php?id=78862
      if [ ! -f php5640-78862.patch ]; then
        echo
        echo "patching PHP 5.6 for bug #78862"
        echo "https://bugs.php.net/bug.php?id=78862"
        echo
        cp -a $CUR_DIR/patches/php/php5640-78862.patch php5640-78862.patch
        dos2unix php5640-78862.patch
        patch -p1 < php5640-78862.patch
      fi
      # PHP 5.6 bug #78863 https://bugs.php.net/bug.php?id=78863
      if [ ! -f php5640-78863.patch ]; then
        echo
        echo "patching PHP 5.6 for bug #78863"
        echo "https://bugs.php.net/bug.php?id=78863"
        echo
        cp -a $CUR_DIR/patches/php/php5640-78863.patch php5640-78863.patch
        dos2unix php5640-78863.patch
        patch -p1 < php5640-78863.patch
      fi
      # PHP 5.6 bug #78793 https://bugs.php.net/bug.php?id=78793
      if [ ! -f php5640-78793.patch ]; then
        echo
        echo "patching PHP 5.6 for bug #78793"
        echo "https://bugs.php.net/bug.php?id=78793"
        echo
        cp -a $CUR_DIR/patches/php/php5640-78793.patch php5640-78793.patch
        dos2unix php5640-78793.patch
        patch -p1 < php5640-78793.patch
      fi
      # PHP 5.6 bug #78878 https://bugs.php.net/bug.php?id=78878
      if [ ! -f php5640-78878.patch ]; then
        echo
        echo "patching PHP 5.6 for bug #78878"
        echo "https://bugs.php.net/bug.php?id=78878"
        echo
        cp -a $CUR_DIR/patches/php/php5640-78878.patch php5640-78878.patch
        dos2unix php5640-78878.patch
        patch -p1 < php5640-78878.patch
      fi
      # PHP 5.6 bug #78910 https://bugs.php.net/bug.php?id=78910
      if [ ! -f php5640-78910.patch ]; then
        echo
        echo "patching PHP 5.6 for bug #78910"
        echo "https://bugs.php.net/bug.php?id=78910"
        echo
        cp -a $CUR_DIR/patches/php/php5640-78910.patch php5640-78910.patch
        dos2unix php5640-78910.patch
        patch -p1 < php5640-78910.patch
      fi
      # PHP 5.6 bug #79037 https://bugs.php.net/bug.php?id=79037
      if [ ! -f php5640-79037.patch ]; then
        echo
        echo "patching PHP 5.6 for bug #79037"
        echo "https://bugs.php.net/bug.php?id=79037"
        echo
        cp -a $CUR_DIR/patches/php/php5640-79037.patch php5640-79037.patch
        dos2unix php5640-79037.patch
        patch -p1 < php5640-79037.patch
      fi
      # PHP 5.6 bug #79099 https://bugs.php.net/bug.php?id=79099
      if [ ! -f php5640-79099.patch ]; then
        echo
        echo "patching PHP 5.6 for bug #79099"
        echo "https://bugs.php.net/bug.php?id=79099"
        echo
        cp -a $CUR_DIR/patches/php/php5640-79099.patch php5640-79099.patch
        dos2unix php5640-79099.patch
        patch -p1 < php5640-79099.patch
      fi
      # PHP 5.6 bug #79082 https://bugs.php.net/bug.php?id=79082
      if [ ! -f php5640-79082.patch ]; then
        echo
        echo "patching PHP 5.6 for bug #79082"
        echo "https://bugs.php.net/bug.php?id=79082"
        echo
        cp -a $CUR_DIR/patches/php/php5640-79082.patch php5640-79082.patch
        dos2unix php5640-79082.patch
        patch -p1 < php5640-79082.patch
      fi
      # PHP 5.6 bug #79221 https://bugs.php.net/bug.php?id=79221
      if [ ! -f php5640-79221.patch ]; then
        echo
        echo "patching PHP 5.6 for bug #79221"
        echo "https://bugs.php.net/bug.php?id=79221"
        echo
        cp -a $CUR_DIR/patches/php/php5640-79221.patch php5640-79221.patch
        dos2unix php5640-79221.patch
        patch -p1 < php5640-79221.patch
      fi
      # php-openssl-cert
      if [ ! -f php5640-php-openssl-cert.patch ]; then
        echo
        echo "patching PHP 5.6 for openssl test certs update"
        echo
        cp -a $CUR_DIR/patches/php/php5640-php-openssl-cert.patch php5640-php-openssl-cert.patch
        dos2unix php5640-php-openssl-cert.patch
        patch -p1 < php5640-php-openssl-cert.patch
      fi
      # PHP 5.6 bug #79282 https://bugs.php.net/bug.php?id=79282
      if [ ! -f php5640-79282.patch ]; then
        echo
        echo "patching PHP 5.6 for bug #79282"
        echo "https://bugs.php.net/bug.php?id=79282"
        echo
        cp -a $CUR_DIR/patches/php/php5640-79282.patch php5640-79282.patch
        dos2unix php5640-79282.patch
        patch -p1 < php5640-79282.patch
      fi
      # PHP 5.6 bug #79329 https://bugs.php.net/bug.php?id=79329
      if [ ! -f php5640-79329.patch ]; then
        echo
        echo "patching PHP 5.6 for bug #79329"
        echo "https://bugs.php.net/bug.php?id=79329"
        echo
        cp -a $CUR_DIR/patches/php/php5640-79329.patch php5640-79329.patch
        dos2unix php5640-79329.patch
        patch -p1 < php5640-79329.patch
      fi
      # PHP 5.6 bug #79330 https://bugs.php.net/bug.php?id=79330
      if [ ! -f php5640-79330.patch ]; then
        echo
        echo "patching PHP 5.6 for bug #79330"
        echo "https://bugs.php.net/bug.php?id=79330"
        echo
        cp -a $CUR_DIR/patches/php/php5640-79330.patch php5640-79330.patch
        dos2unix php5640-79330.patch
        patch -p1 < php5640-79330.patch
      fi
      # PHP 5.6 bug #79465 https://bugs.php.net/bug.php?id=79465
      if [ ! -f php5640-79465.patch ]; then
        echo
        echo "patching PHP 5.6 for bug #79465"
        echo "https://bugs.php.net/bug.php?id=79465"
        echo
        cp -a $CUR_DIR/patches/php/php5640-79465.patch php5640-79465.patch
        dos2unix php5640-79465.patch
        patch -p1 < php5640-79465.patch
      fi
      # PHP 5.6 bug #78875 https://bugs.php.net/bug.php?id=78875
      # PHP 5.6 bug #78876 https://bugs.php.net/bug.php?id=78876
      if [ ! -f php5640-78875.patch ]; then
        echo
        echo "patching PHP 5.6 for bug #78875"
        echo "https://bugs.php.net/bug.php?id=78875"
        echo
        cp -a $CUR_DIR/patches/php/php5640-78875.patch php5640-78875.patch
        dos2unix php5640-78875.patch
        patch -p1 < php5640-78875.patch
      fi
      # PHP 5.6 bug #79797 https://bugs.php.net/bug.php?id=79797
      if [ ! -f php5640-79797.patch ]; then
        echo
        echo "patching PHP 5.6 for bug #79797"
        echo "https://bugs.php.net/bug.php?id=79797"
        echo
        cp -a $CUR_DIR/patches/php/php5640-79797.patch php5640-79797.patch
        dos2unix php5640-79797.patch
        patch -p1 < php5640-79797.patch
      fi
      # PHP 5.6 bug #79877 https://bugs.php.net/bug.php?id=79877
      if [ ! -f php5640-79877.patch ]; then
        echo
        echo "patching PHP 5.6 for bug #79877"
        echo "https://bugs.php.net/bug.php?id=79877"
        echo
        cp -a $CUR_DIR/patches/php/php5640-79877.patch php5640-79877.patch
        dos2unix php5640-79877.patch
        patch -p1 < php5640-79877.patch
      fi
      # PHP 5.6 bug #79699 https://bugs.php.net/bug.php?id=79699
      if [ ! -f php5640-79699.patch ]; then
        echo
        echo "patching PHP 5.6 for bug #79699"
        echo "https://bugs.php.net/bug.php?id=79699"
        echo
        cp -a $CUR_DIR/patches/php/php5640-79699.patch php5640-79699.patch
        dos2unix php5640-79699.patch
        patch -p1 < php5640-79699.patch
      fi
      if [[ ! -f php5640-mysqlnd-fix.patch && "$PHP_MYSQLND_PATCH_FIX" = [yY] ]]; then
        echo
        echo "patching PHP 7.0.33 backport mysqlnd fix from PHP 7.3 for MariaDB issue https://jira.mariadb.org/browse/MDEV-24121"
        echo
        cp -a $CUR_DIR/patches/php/php5640-mysqlnd-fix.patch php5640-mysqlnd-fix.patch
        dos2unix php5640-mysqlnd-fix.patch
        patch -p1 < php5640-mysqlnd-fix.patch
      fi
      # PHP 5.6 bug #77423 https://bugs.php.net/bug.php?id=77423
      if [ ! -f php5640-77423.patch ]; then
        echo
        echo "patching PHP 5.6 for bug #77423"
        echo "https://bugs.php.net/bug.php?id=77423"
        echo
        cp -a $CUR_DIR/patches/php/php5640-77423.patch php5640-77423.patch
        dos2unix php5640-77423.patch
        patch -p1 < php5640-77423.patch
      fi
      # PHP 5.6 bug #80672 https://bugs.php.net/bug.php?id=80672
      if [ ! -f php5640-80672.patch ]; then
        echo
        echo "patching PHP 5.6 for bug #80672"
        echo "https://bugs.php.net/bug.php?id=80672"
        echo
        cp -a $CUR_DIR/patches/php/php5640-80672.patch php5640-80672.patch
        dos2unix php5640-80672.patch
        patch -p1 < php5640-80672.patch
      fi
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
    if [[ "$PHPMUVER" = '7.0' || "$PHPMVER" = '7.0' ]] && [[ "$PHPVER_ID" -ge '7033' ]]; then
      # backport security bug patch from PHP 7.1. to 7.0.33
      # PHP bug #77369 https://bugs.php.net/bug.php?id=77369
      # PHP bug #77370 https://bugs.php.net/bug.php?id=77370
      # PHP bug #77371 https://bugs.php.net/bug.php?id=77371
      # PHP bug #77381 https://bugs.php.net/bug.php?id=77381
      # PHP bug #77382 https://bugs.php.net/bug.php?id=77382
      # PHP bug #77418 https://bugs.php.net/bug.php?id=77418
      # PHP bug #77247 https://bugs.php.net/bug.php?id=77247
      # PHP bug #77242 https://bugs.php.net/bug.php?id=77242
      # PHP bug #77380 https://bugs.php.net/bug.php?id=77380
      # PHP bug #77540 https://bugs.php.net/bug.php?id=77540
      # PHP bug #77563 https://bugs.php.net/bug.php?id=77563
      # PHP bug #77396 https://bugs.php.net/bug.php?id=77396
      # PHP bug #77431 https://bugs.php.net/bug.php?id=77431
      # PHP bug #77630 https://bugs.php.net/bug.php?id=77630
      # PHP bug #76846 https://bugs.php.net/bug.php?id=76846
      # PHP bug #76846 https://bugs.php.net/bug.php?id=77950
      # PHP bug #77967 https://bugs.php.net/bug.php?id=77967
      # PHP bug #77988 https://bugs.php.net/bug.php?id=77988
      # PHP bug #78069 https://bugs.php.net/bug.php?id=78069
      # PHP bug #77919 https://bugs.php.net/bug.php?id=77919
      # PHP bug #78222 https://bugs.php.net/bug.php?id=78222
      # PHP bug #78256 https://bugs.php.net/bug.php?id=78256
      # PHP bug #75457 https://bugs.php.net/bug.php?id=75457
      # PHP bug #78380 https://bugs.php.net/bug.php?id=78380
      # PHP bug #78599 https://bugs.php.net/bug.php?id=78599
      # PHP bug #78862 https://bugs.php.net/bug.php?id=78862
      # PHP bug #78863 https://bugs.php.net/bug.php?id=78863
      # PHP bug #78793 https://bugs.php.net/bug.php?id=78793
      # PHP bug #78878 https://bugs.php.net/bug.php?id=78878
      # PHP bug #78910 https://bugs.php.net/bug.php?id=78910
      # PHP bug #79037 https://bugs.php.net/bug.php?id=79037
      # PHP bug #79099 https://bugs.php.net/bug.php?id=79099
      # PHP bug #77569 https://bugs.php.net/bug.php?id=77569
      # PHP bug #79221 https://bugs.php.net/bug.php?id=79221
      # PHP bug #79082 https://bugs.php.net/bug.php?id=79082
      # PHP bug #79282 https://bugs.php.net/bug.php?id=79282
      # PHP bug #79329 https://bugs.php.net/bug.php?id=79329
      # PHP bug #79330 https://bugs.php.net/bug.php?id=79330
      # PHP bug #79465 https://bugs.php.net/bug.php?id=79465
      # PHP bug #78875 https://bugs.php.net/bug.php?id=78875
      # PHP bug #78876 https://bugs.php.net/bug.php?id=78876
      # PHP bug #79797 https://bugs.php.net/bug.php?id=79797
      # PHP bug #79877 https://bugs.php.net/bug.php?id=79877
      # PHP bug #79699 https://bugs.php.net/bug.php?id=79699
      # PHP bug #77423 https://bugs.php.net/bug.php?id=77423
      phpseven_bugids='77369 77370 77371 77381 77418 77247 77242 77380 77540 77563 77396 77431 77630 76846 php-openssl-cert 77753 77831 sqlite3-defensive 77950 77967 77988 78069 77919 78222 78256 75457 78380 78599 78862 78863 78793 78878 78910 79037 79099 77569 79221 79082 php-openssl-cert2 79282 79329 79330 79465 78875 78876 79797 79877 79699 mysqlnd-fix 77423 80672'
      for bugid in $phpseven_bugids; do
        if [[ ! -f "php7033-${bugid}.patch" && -f "$CUR_DIR/patches/php/php7033-${bugid}.patch" ]]; then
          echo
          if [[ "$bugid" != 'php-openssl-cert' ]]; then
            echo "patching PHP 7.0.33+ for bug #${bugid}"
            echo "https://bugs.php.net/bug.php?id=${bugid}"
          elif [[ "$bugid" = 'sqlite3-defensive' ]]; then
            echo "patching PHP 7.0.33+ sqlite3-defensive patch"
          elif [[ "$bugid" = 'php-openssl-cert' ]]; then
            echo "patching PHP 7.0.33+ php-openssl-cert patch"
          elif [[ "$bugid" = 'php-openssl-cert2' ]]; then
            echo "patching PHP 7.0.33+ php-openssl-cert patch #2"
          elif [[ "$bugid" = 'mysqlnd-fix' && "$PHP_MYSQLND_PATCH_FIX" = [yY] ]]; then
            echo "patching PHP 7.0.33 backport mysqlnd fix from PHP 7.3 for MariaDB issue https://jira.mariadb.org/browse/MDEV-24121"
            echo
            cp -a $CUR_DIR/patches/php/php7033-${bugid}.patch php7033-${bugid}.patch
            dos2unix php7033-${bugid}.patch
            patch -p1 < php7033-${bugid}.patch
          fi
          if [[ "$bugid" != 'mysqlnd-fix' ]]; then
            echo
            cp -a $CUR_DIR/patches/php/php7033-${bugid}.patch php7033-${bugid}.patch
            dos2unix php7033-${bugid}.patch
            patch -p1 < php7033-${bugid}.patch
          fi
        fi
      done
    fi
    if [[ "$PHPMUVER" = '7.1' || "$PHPMVER" = '7.1' ]] && [[ "$PHPVER_ID" -ge '7133' ]]; then
      # backport security bug patch from PHP 7.3. to 7.1.33
      # PHP bug #78862 https://bugs.php.net/bug.php?id=78862
      # PHP bug #78863 https://bugs.php.net/bug.php?id=78863
      # PHP bug #78793 https://bugs.php.net/bug.php?id=78793
      # PHP bug #78878 https://bugs.php.net/bug.php?id=78878
      # PHP bug #78910 https://bugs.php.net/bug.php?id=78910
      # PHP bug #79037 https://bugs.php.net/bug.php?id=79037
      # PHP bug #79091 https://bugs.php.net/bug.php?id=79091
      # PHP bug #79099 https://bugs.php.net/bug.php?id=79099
      # PHP bug #77569 https://bugs.php.net/bug.php?id=77569
      # PHP bug #79221 https://bugs.php.net/bug.php?id=79221
      # PHP bug #79082 https://bugs.php.net/bug.php?id=79082
      # PHP bug #79282 https://bugs.php.net/bug.php?id=79282
      # PHP bug #79329 https://bugs.php.net/bug.php?id=79329
      # PHP bug #79330 https://bugs.php.net/bug.php?id=79330
      # PHP bug #79465 https://bugs.php.net/bug.php?id=79465
      # PHP bug #78875 https://bugs.php.net/bug.php?id=78875
      # PHP bug #78876 https://bugs.php.net/bug.php?id=78876
      # PHP bug #79797 https://bugs.php.net/bug.php?id=79797
      # PHP bug #79877 https://bugs.php.net/bug.php?id=79877
      # PHP bug #79699 https://bugs.php.net/bug.php?id=79699
      # PHP bug #79601 https://bugs.php.net/bug.php?id=79601
      # PHP bug #77423 https://bugs.php.net/bug.php?id=77423
      # PHP bug #80672 https://bugs.php.net/bug.php?id=80672
      phpseven_bugids='78862 78863 78793 78878 78910 79037 79091 79099 77569 79221 79082 php-openssl-cert 79282 79329 79330 79465 78875 78876 79797 79877 79699 79601 mysqlnd-fix 77423 80672'
      for bugid in $phpseven_bugids; do
        if [[ ! -f "php7133-${bugid}.patch" && -f "$CUR_DIR/patches/php/php7133-${bugid}.patch" ]]; then
          echo
          if [[ "$bugid" != 'php-openssl-cert' ]]; then
            echo "patching PHP 7.1.33+ for bug #${bugid}"
            echo "https://bugs.php.net/bug.php?id=${bugid}"
          elif [[ "$bugid" = 'sqlite3-defensive' ]]; then
            echo "patching PHP 7.1.33+ sqlite3-defensive patch"
          elif [[ "$bugid" = 'php-openssl-cert' ]]; then
            echo "patching PHP 7.1.33+ php-openssl-cert patch"
          elif [[ "$bugid" = 'mysqlnd-fix' && "$PHP_MYSQLND_PATCH_FIX" = [yY] ]]; then
            echo "patching PHP 7.1.33 backport mysqlnd fix from PHP 7.3 for MariaDB issue https://jira.mariadb.org/browse/MDEV-24121"
            echo
            cp -a $CUR_DIR/patches/php/php7133-${bugid}.patch php7133-${bugid}.patch
            dos2unix php7133-${bugid}.patch
            patch -p1 < php7133-${bugid}.patch
          fi
          if [[ "$bugid" != 'mysqlnd-fix' ]]; then
            echo
            cp -a $CUR_DIR/patches/php/php7133-${bugid}.patch php7133-${bugid}.patch
            dos2unix php7133-${bugid}.patch
            patch -p1 < php7133-${bugid}.patch
          fi
        fi
      done
    fi
    if [[ "$PHPVER_ID" -ge '70200' && "$PHPVER_ID" -lt '70300' ]]; then
      phpseven_bugids='mysqlnd-fix'
      for bugid in $phpseven_bugids; do
        if [[ ! -f "php72-${bugid}.patch" && -f "$CUR_DIR/patches/php/php72-${bugid}.patch" ]]; then
          echo
          if [[ "$bugid" = 'mysqlnd-fix' && "$PHP_MYSQLND_PATCH_FIX" = [yY] ]]; then
            echo "patching PHP 7.2.x branch backport mysqlnd fix from PHP 7.3 for MariaDB issue https://jira.mariadb.org/browse/MDEV-24121"
            echo
            cp -a $CUR_DIR/patches/php/php72-${bugid}.patch php72-${bugid}.patch
            dos2unix php72-${bugid}.patch
            patch -p1 < php72-${bugid}.patch
          fi
          if [[ "$bugid" != 'mysqlnd-fix' ]]; then
            echo
            cp -a $CUR_DIR/patches/php/php72-${bugid}.patch php72-${bugid}.patch
            dos2unix php72-${bugid}.patch
            patch -p1 < php72-${bugid}.patch
          fi
        fi
      done
    fi
    if [[ "$PHPMUVER" = '7.2' || "$PHPMVER" = '7.2' ]] && [[ "$PHPVER_ID" -ge '7234' ]]; then
      # backport security bug patch from PHP 7.3. to 7.2.34
      # PHP bug #77423 https://bugs.php.net/bug.php?id=77423
      # PHP bug #80672 https://bugs.php.net/bug.php?id=80672
      phpseven_bugids='77423 80672'
      for bugid in $phpseven_bugids; do
        if [[ ! -f "php7234-${bugid}.patch" && -f "$CUR_DIR/patches/php/php7234-${bugid}.patch" ]]; then
          echo
          if [[ "$bugid" != 'php-openssl-cert' ]]; then
            echo "patching PHP 7.1.33+ for bug #${bugid}"
            echo "https://bugs.php.net/bug.php?id=${bugid}"
          elif [[ "$bugid" = 'sqlite3-defensive' ]]; then
            echo "patching PHP 7.1.33+ sqlite3-defensive patch"
          elif [[ "$bugid" = 'php-openssl-cert' ]]; then
            echo "patching PHP 7.1.33+ php-openssl-cert patch"
          elif [[ "$bugid" = 'mysqlnd-fix' && "$PHP_MYSQLND_PATCH_FIX" = [yY] ]]; then
            echo "patching PHP 7.1.33 backport mysqlnd fix from PHP 7.3 for MariaDB issue https://jira.mariadb.org/browse/MDEV-24121"
            echo
            cp -a $CUR_DIR/patches/php/php7234-${bugid}.patch php7234-${bugid}.patch
            dos2unix php7234-${bugid}.patch
            patch -p1 < php7234-${bugid}.patch
          fi
          if [[ "$bugid" != 'mysqlnd-fix' ]]; then
            echo
            cp -a $CUR_DIR/patches/php/php7234-${bugid}.patch php7234-${bugid}.patch
            dos2unix php7234-${bugid}.patch
            patch -p1 < php7234-${bugid}.patch
          fi
        fi
      done
    fi
    if [[ "$PHPVER_ID" -ge '70304' && "$PHPVER_ID" -le '70306' ]]; then
      # backport security bug patch for php 7.3
      # fix lands in PHP 7.3.7 so backport for 7.3.4 to 7.3.6 https://bugs.php.net/bug.php?id=77955
      phpseven_bugids='77955'
      for bugid in $phpseven_bugids; do
        if [[ ! -f "php735-${bugid}.patch" && -f "$CUR_DIR/patches/php/php735-${bugid}.patch" ]]; then
          echo
          if [[ "$bugid" != 'php-openssl-cert' ]]; then
            echo "patching PHP 7.3.4+ for bug #${bugid}"
            echo "https://bugs.php.net/bug.php?id=${bugid}"
          fi
          echo
          cp -a $CUR_DIR/patches/php/php735-${bugid}.patch php735-${bugid}.patch
          dos2unix php735-${bugid}.patch
          patch -p1 < php735-${bugid}.patch
        fi
      done
    fi
  fi
  if [[ "$FPM_WAKEUP_PATCH" = [yY] && -f "$CUR_DIR/patches/php/intel/reduce-fpm-event-wakeups.patch" ]]; then
    echo
    echo "PHP-FPM patch to reduce event wakeups"
    echo
    cp -a "$CUR_DIR/patches/php/intel/reduce-fpm-event-wakeups.patch" reduce-fpm-event-wakeups.patch
    dos2unix reduce-fpm-event-wakeups.patch
    patch -p1 < reduce-fpm-event-wakeups.patch
  fi
  if [[ "$PHPVER_ID" -eq '70402' ]]; then
    # echo
    # echo "revert PHP 7.4.2 main/php_variables.c to PHP 7.4.1 version"
    # echo "https://community.centminmod.com/posts/81184/"
    # wget -4 https://raw.githubusercontent.com/php/php-src/PHP-7.4.1/main/php_variables.c -O main/php_variables.c

    # backport security bug patch for php 7.4
    # 79174 https://community.centminmod.com/posts/81231/
    phpseven_bugids='79174'
    for bugid in $phpseven_bugids; do
      if [[ ! -f "php742-${bugid}.patch" && -f "$CUR_DIR/patches/php/php742-${bugid}.patch" ]]; then
        echo
        if [[ "$bugid" != 'php-openssl-cert' ]]; then
          echo "patching PHP 7.4.2 for bug #${bugid}"
          echo "https://bugs.php.net/bug.php?id=${bugid}"
        fi
        echo
        cp -a $CUR_DIR/patches/php/php742-${bugid}.patch php742-${bugid}.patch
        dos2unix php742-${bugid}.patch
        patch -p1 < php742-${bugid}.patch
      fi
    done
  fi
} 2>&1 | tee -a "${CENTMINLOGDIR}/patch_php_${DT}.log"
}