imagick_php_patches() {
  echo "imagick patch routine: PHPMVER=$PHPMVER"
  echo "imagick patch routine: PHPMUVER=$PHPMUVER"
  echo "imagick patch routine: PHPVER_ID=$PHPVER_ID"
  if [[ "$PHPMVER" = '8.4' || "$PHPMUVER" = '8.4' || "$PHPVER_ID" -ge '80400' ]]; then
    # Define the new patch for imagick PHP 8.4 support
    IMAGICK_PATCH="php84-remove-strtolower.patch"
    echo "$CUR_DIR/patches/imagick/$IMAGICK_PATCH"
    if [ -f "$CUR_DIR/patches/imagick/$IMAGICK_PATCH" ]; then
      echo
      echo "Patching imagick for PHP 8.4 support"
      cp -a "$CUR_DIR/patches/imagick/$IMAGICK_PATCH" "$IMAGICK_PATCH"
      dos2unix "$IMAGICK_PATCH"

      # Attempt to apply the patch
      if patch -p1 < "$IMAGICK_PATCH"; then
        echo "Patch applied successfully for imagick PHP 8.4 support"
      else
        echo "Patching failed for imagick PHP 8.4 support"
        # Create a temporary file to store list of patched files
        temp_file=$(mktemp)

        # Extract the filenames from the patch to determine which files to restore
        grep -oP '(?<=\+\+\+ )[^\s]+' "$IMAGICK_PATCH" > "$temp_file"

        # Read each file from the temporary file and restore the original if patch failed
        while read -r file; do
          # Remove any 'b/' or other prefixes from the filename
          file=$(echo "$file" | sed 's/b\///')
          
          # Check if the original (.orig) and reject (.rej) files exist, and restore them if needed
          if [[ -f "$file.orig" && -f "$file.rej" ]]; then
            echo "Restoring original file: $file"
            mv -f "$file.orig" "$file"
            rm "$file.rej"
          fi
        done < "$temp_file"

        # Remove the temporary file
        rm "$temp_file"
      fi
    else
      echo "$IMAGICK_PATCH patch file already applied or not available"
    fi
  else
    echo "skipped imagick_php_patches function"
  fi
}

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
    if [[ "$PHPMUVER" = '5.4' || "$PHPMVER" = '5.4' ]]; then
      # backport security bug patch from PHP 7.4.30 to 
      # PHP 5.4 bug #81719 https://bugs.php.net/bug.php?id=81719
      if [ ! -f php54-81719.patch ]; then
        echo
        echo "patching PHP 5.4 for bug #81719"
        echo "https://bugs.php.net/bug.php?id=81719"
        echo
        cp -a $CUR_DIR/patches/php/php54-81719.patch php54-81719.patch
        dos2unix php54-81719.patch
        patch -p1 < php54-81719.patch
      fi
    fi
    if [[ "$PHPMUVER" = '5.5' || "$PHPMVER" = '5.5' ]]; then
      # backport security bug patch from PHP 7.4.30 to 
      # PHP 5.5 bug #81719 https://bugs.php.net/bug.php?id=81719
      if [ ! -f php55-81719.patch ]; then
        echo
        echo "patching PHP 5.5 for bug #81719"
        echo "https://bugs.php.net/bug.php?id=81719"
        echo
        cp -a $CUR_DIR/patches/php/php55-81719.patch php55-81719.patch
        dos2unix php55-81719.patch
        patch -p1 < php55-81719.patch
      fi
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
      # PHP 5.6 bug #80710 https://bugs.php.net/bug.php?id=80710
      if [ ! -f php5640-80710.patch ]; then
        echo
        echo "patching PHP 5.6 for bug #80710"
        echo "https://bugs.php.net/bug.php?id=80710"
        echo
        cp -a $CUR_DIR/patches/php/php5640-80710.patch php5640-80710.patch
        dos2unix php5640-80710.patch
        patch -p1 < php5640-80710.patch
      fi
      # PHP 5.6 bug #81122 https://bugs.php.net/bug.php?id=81122
      if [ ! -f php5640-81122.patch ]; then
        echo
        echo "patching PHP 5.6 for bug #81122"
        echo "https://bugs.php.net/bug.php?id=81122"
        echo
        cp -a $CUR_DIR/patches/php/php5640-81122.patch php5640-81122.patch
        dos2unix php5640-81122.patch
        patch -p1 < php5640-81122.patch
      fi
      # PHP 5.6 bug #76488 https://bugs.php.net/bug.php?id=76488
      if [ ! -f php5640-76488.patch ]; then
        echo
        echo "patching PHP 5.6 for bug #76488"
        echo "https://bugs.php.net/bug.php?id=76488"
        echo
        cp -a $CUR_DIR/patches/php/php5640-76488.patch php5640-76488.patch
        dos2unix php5640-76488.patch
        patch -p1 < php5640-76488.patch
      fi
      # PHP 5.6 bug #81211 https://bugs.php.net/bug.php?id=81211
      if [ ! -f php5640-81211.patch ]; then
        echo
        echo "patching PHP 5.6 for bug #81211"
        echo "https://bugs.php.net/bug.php?id=81211"
        echo
        cp -a $CUR_DIR/patches/php/php5640-81211.patch php5640-81211.patch
        dos2unix php5640-81211.patch
        patch -p1 < php5640-81211.patch
      fi
      # PHP 5.6 bug #81026 https://bugs.php.net/bug.php?id=81026
      if [ ! -f php5640-81026.patch ]; then
        echo
        echo "patching PHP 5.6 for bug #81026"
        echo "https://bugs.php.net/bug.php?id=81026"
        echo
        cp -a $CUR_DIR/patches/php/php5640-81026.patch php5640-81026.patch
        dos2unix php5640-81026.patch
        patch -p1 < php5640-81026.patch
      fi
      # PHP 5.6 bug #79971 https://bugs.php.net/bug.php?id=79971
      if [ ! -f php5640-79971.patch ]; then
        echo
        echo "patching PHP 5.6 for bug #79971"
        echo "https://bugs.php.net/bug.php?id=79971"
        echo
        cp -a $CUR_DIR/patches/php/php5640-79971.patch php5640-79971.patch
        dos2unix php5640-79971.patch
        patch -p1 < php5640-79971.patch
      fi
      # PHP 5.6 bug #81719 https://bugs.php.net/bug.php?id=81719
      if [ ! -f php5640-81719.patch ]; then
        echo
        echo "patching PHP 5.6 for bug #81719"
        echo "https://bugs.php.net/bug.php?id=81719"
        echo
        cp -a $CUR_DIR/patches/php/php5640-81719.patch php5640-81719.patch
        dos2unix php5640-81719.patch
        patch -p1 < php5640-81719.patch
      fi
      # PHP 5.6 bug #81720 https://bugs.php.net/bug.php?id=81720
      if [ ! -f php5640-81720.patch ]; then
        echo
        echo "patching PHP 5.6 for bug #81720"
        echo "https://bugs.php.net/bug.php?id=81720"
        echo
        cp -a $CUR_DIR/patches/php/php5640-81720.patch php5640-81720.patch
        dos2unix php5640-81720.patch
        patch -p1 < php5640-81720.patch
      fi
      # PHP 5.6 bug #81726 https://bugs.php.net/bug.php?id=81726
      if [ ! -f php5640-81726.patch ]; then
        echo
        echo "patching PHP 5.6 for bug #81726"
        echo "https://bugs.php.net/bug.php?id=81726"
        echo
        cp -a $CUR_DIR/patches/php/php5640-81726.patch php5640-81726.patch
        dos2unix php5640-81726.patch
        patch -p1 < php5640-81726.patch
      fi
      # PHP 5.6 bug #81727 https://bugs.php.net/bug.php?id=81727
      if [ ! -f php5640-81727.patch ]; then
        echo
        echo "patching PHP 5.6 for bug #81727"
        echo "https://bugs.php.net/bug.php?id=81727"
        echo
        cp -a $CUR_DIR/patches/php/php5640-81727.patch php5640-81727.patch
        dos2unix php5640-81727.patch
        patch -p1 < php5640-81727.patch
      fi
      # PHP 5.6 bug #81740 https://bugs.php.net/bug.php?id=81740
      if [ ! -f php5640-81740.patch ]; then
        echo
        echo "patching PHP 5.6 for bug #81740"
        echo "https://bugs.php.net/bug.php?id=81740"
        echo
        cp -a $CUR_DIR/patches/php/php5640-81740.patch php5640-81740.patch
        dos2unix php5640-81740.patch
        patch -p1 < php5640-81740.patch
      fi
      if [ ! -f php5640-81744.patch ]; then
        echo
        echo "patching PHP 5.6 for bug #81744"
        echo "https://bugs.php.net/bug.php?id=81744"
        echo
        cp -a $CUR_DIR/patches/php/php5640-81744.patch php5640-81744.patch
        dos2unix php5640-81744.patch
        patch -p1 < php5640-81744.patch
      fi
      if [ ! -f php5640-81746.patch ]; then
        echo
        echo "patching PHP 5.6 for bug #81746"
        echo "https://bugs.php.net/bug.php?id=81746"
        echo
        cp -a $CUR_DIR/patches/php/php5640-81746.patch php5640-81746.patch
        dos2unix php5640-81746.patch
        patch -p1 < php5640-81746.patch
      fi
      if [ ! -f php5640-cve-2023-0662.patch ]; then
        echo
        echo "patching PHP 5.6 for bug #cve-2023-0662"
        echo "https://bugs.php.net/bug.php?id=cve-2023-0662"
        echo
        cp -a $CUR_DIR/patches/php/php5640-cve-2023-0662.patch php5640-cve-2023-0662.patch
        dos2unix php5640-cve-2023-0662.patch
        patch -p1 < php5640-cve-2023-0662.patch
      fi
      if [ ! -f php5640-ghsa-76gg-c692-v2mw.patch ]; then
        echo
        echo "patching PHP 5.6 ghsa-76gg-c692-v2mw"
        echo
        cp -a $CUR_DIR/patches/php/php5640-ghsa-76gg-c692-v2mw.patch php5640-ghsa-76gg-c692-v2mw.patch
        dos2unix php5640-ghsa-76gg-c692-v2mw.patch
        patch -p1 < php5640-ghsa-76gg-c692-v2mw.patch
      fi
      if [ ! -f php5640-cve-2023-3823.patch ]; then
        echo
        echo "patching PHP 5.6 for bug #cve-2023-3823"
        echo "https://bugs.php.net/bug.php?id=cve-2023-3823"
        echo
        cp -a $CUR_DIR/patches/php/php5640-cve-2023-3823.patch php5640-cve-2023-3823.patch
        dos2unix php5640-cve-2023-3823.patch
        patch -p1 < php5640-cve-2023-3823.patch
      fi
      if [ ! -f php5640-cve-2023-3824.patch ]; then
        echo
        echo "patching PHP 5.6 for bug #cve-2023-3824"
        echo "https://bugs.php.net/bug.php?id=cve-2023-3824"
        echo
        cp -a $CUR_DIR/patches/php/php5640-cve-2023-3824.patch php5640-cve-2023-3824.patch
        dos2unix php5640-cve-2023-3824.patch
        patch -p1 < php5640-cve-2023-3824.patch
      fi
      if [ ! -f php5640-cve-2024-2756.patch ]; then
        echo
        echo "patching PHP 5.6 for bug #cve-2024-2756"
        echo "https://bugs.php.net/bug.php?id=cve-2024-2756"
        echo
        cp -a $CUR_DIR/patches/php/php5640-cve-2024-2756.patch php5640-cve-2024-2756.patch
        dos2unix php5640-cve-2024-2756.patch
        patch -p1 < php5640-cve-2024-2756.patch
      fi
      if [ ! -f php5640-cve-2024-3096.patch ]; then
        echo
        echo "patching PHP 5.6 for bug #cve-2024-3096"
        echo "https://bugs.php.net/bug.php?id=cve-2024-3096"
        echo
        cp -a $CUR_DIR/patches/php/php5640-cve-2024-3096.patch php5640-cve-2024-3096.patch
        dos2unix php5640-cve-2024-3096.patch
        patch -p1 < php5640-cve-2024-3096.patch
      fi
      if [ ! -f php5640-cve-2024-5458.patch ]; then
        echo
        echo "patching PHP 5.6 for bug #cve-2024-5458"
        echo "https://bugs.php.net/bug.php?id=cve-2024-5458"
        echo
        cp -a $CUR_DIR/patches/php/php5640-cve-2024-5458.patch php5640-cve-2024-5458.patch
        dos2unix php5640-cve-2024-5458.patch
        patch -p1 < php5640-cve-2024-5458.patch
      fi
      if [ ! -f php5640-cve-2024-8925.patch ]; then
        echo
        echo "patching PHP 5.6 for bug #cve-2024-8925"
        echo "https://bugs.php.net/bug.php?id=cve-2024-8925"
        echo
        cp -a $CUR_DIR/patches/php/php5640-cve-2024-8925.patch php5640-cve-2024-8925.patch
        dos2unix php5640-cve-2024-8925.patch
        patch -p1 < php5640-cve-2024-8925.patch
      fi
      if [ ! -f php5640-cve-2024-8926.patch ]; then
        echo
        echo "patching PHP 5.6 for bug #cve-2024-8926"
        echo "https://bugs.php.net/bug.php?id=cve-2024-8926"
        echo
        cp -a $CUR_DIR/patches/php/php5640-cve-2024-8926.patch php5640-cve-2024-8926.patch
        dos2unix php5640-cve-2024-8926.patch
        patch -p1 < php5640-cve-2024-8926.patch
      fi
      if [ ! -f php5640-cve-2024-8927.patch ]; then
        echo
        echo "patching PHP 5.6 for bug #cve-2024-8927"
        echo "https://bugs.php.net/bug.php?id=cve-2024-8927"
        echo
        cp -a $CUR_DIR/patches/php/php5640-cve-2024-8927.patch php5640-cve-2024-8927.patch
        dos2unix php5640-cve-2024-8927.patch
        patch -p1 < php5640-cve-2024-8927.patch
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
    if [[ "$PHPMUVER" = '7.0' || "$PHPMVER" = '7.0' ]] && [[ "$PHPVER_ID" -ge '70000' && "$PHPVER_ID" -le '70033' ]]; then
      # backport security bug patch from PHP 7.1. to 7.0.33
      phpseven_bugids='77369 77370 77371 77381 77418 77247 77242 77380 77540 77563 77396 77431 77630 76846 php-openssl-cert 77753 77831 sqlite3-defensive 77950 77967 77988 78069 77919 78222 78256 75457 78380 78599 78862 78863 78793 78878 78910 79037 79099 77569 79221 79082 php-openssl-cert2 79282 79329 79330 79465 78875 78876 79797 79877 79699 mysqlnd-fix 77423 80672 80710 81122 76488 81211 81026 79971 81719 81720 81726 81727 81740 81744 81746 cve-2023-0662 ghsa-76gg-c692-v2mw cve-2023-3823 cve-2023-3824 cve-2024-2756 cve-2024-3096 cve-2024-5458 cve-2024-8925 cve-2024-8926 cve-2024-8927'
      for bugid in $phpseven_bugids; do
        if [[ ! -f "php7033-${bugid}.patch" && -f "$CUR_DIR/patches/php/php7033-${bugid}.patch" ]]; then
          echo
          if [[ "$bugid" != 'php-openssl-cert' ]]; then
            echo "Patching PHP 7.0.33+ for bug #${bugid}"
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
            # Copy the patch file
            cp -a $CUR_DIR/patches/php/php7033-${bugid}.patch php7033-${bugid}.patch
            dos2unix php7033-${bugid}.patch
            # Attempt to apply the patch
            if patch -p1 < php7033-${bugid}.patch; then
              echo "Patch applied successfully for bug #${bugid}"
            else
              echo "Patching failed for bug #${bugid}"
              # Check each target file for .orig and .rej files
              # Create a temporary file
              temp_file=$(mktemp)

              # Use grep to find the necessary information and store it in the temporary file
              grep -oP '(?<=\+\+\+ )[^\s]+' "php7033-${bugid}.patch" > "$temp_file"

              # Read from the temporary file
              while read -r file; do
                # Remove the 'b/' prefix from the filename
                file=$(echo "$file" | sed 's/b\///')
                # Check if the original and reject files exist
                if [[ -f "$file.orig" && -f "$file.rej" ]]; then
                  echo "Restoring original file: $file"
                  mv -f "$file.orig" "$file"
                  rm "$file.rej"
                fi
              done < "$temp_file"

              # Remove the temporary file
              rm "$temp_file"
            fi
          fi
        else
          echo "php7033-${bugid}.patch patch file already applied"
        fi
      done
    fi
    if [[ "$PHPMUVER" = '7.1' || "$PHPMVER" = '7.1' ]] && [[ "$PHPVER_ID" -ge '70100' && "$PHPVER_ID" -le '70133' ]]; then
      # backport security bug patch from PHP 7.3. to 7.1.33
      phpseven_bugids='78862 78863 78793 78878 78910 79037 79091 79099 77569 79221 79082 php-openssl-cert 79282 79329 79330 79465 78875 78876 79797 79877 79699 79601 mysqlnd-fix 77423 80672 80710 81122 76452 81211 81026 79971 81719 81720 81726 81727 81740 81744 81746 cve-2023-0662 ghsa-76gg-c692-v2mw cve-2023-3823 cve-2023-3824 cve-2024-2756 cve-2024-3096 cve-2024-5458 cve-2024-8925 cve-2024-8926 cve-2024-8927'
      for bugid in $phpseven_bugids; do
        if [[ ! -f "php7133-${bugid}.patch" && -f "$CUR_DIR/patches/php/php7133-${bugid}.patch" ]]; then
          echo
          if [[ "$bugid" != 'php-openssl-cert' ]]; then
            echo "Patching PHP 7.1.33+ for bug #${bugid}"
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
            # Copy the patch file
            cp -a $CUR_DIR/patches/php/php7133-${bugid}.patch php7133-${bugid}.patch
            dos2unix php7133-${bugid}.patch
            # Attempt to apply the patch
            if patch -p1 < php7133-${bugid}.patch; then
              echo "Patch applied successfully for bug #${bugid}"
            else
              echo "Patching failed for bug #${bugid}"
              # Check each target file for .orig and .rej files
              # Create a temporary file
              temp_file=$(mktemp)

              # Use grep to find the necessary information and store it in the temporary file
              grep -oP '(?<=\+\+\+ )[^\s]+' "php7133-${bugid}.patch" > "$temp_file"

              # Read from the temporary file
              while read -r file; do
                # Remove the 'b/' prefix from the filename
                file=$(echo "$file" | sed 's/b\///')
                # Check if the original and reject files exist
                if [[ -f "$file.orig" && -f "$file.rej" ]]; then
                  echo "Restoring original file: $file"
                  mv -f "$file.orig" "$file"
                  rm "$file.rej"
                fi
              done < "$temp_file"

              # Remove the temporary file
              rm "$temp_file"
            fi
          fi
        else
          echo "php7133-${bugid}.patch patch file already applied"
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
    if [[ "$PHPMUVER" = '7.2' || "$PHPMVER" = '7.2' ]] && [[ "$PHPVER_ID" -ge '70200' && "$PHPVER_ID" -le '70234' ]]; then
      # backport security bug patch from PHP 7.3. to 7.2.34
      phpseven_bugids='77423 80672 80710 81122 76452 81211 81026 79971 81719 81720 81726 81727 81738 81740 81744 81746 cve-2023-0662 ghsa-76gg-c692-v2mw cve-2023-3823 cve-2023-3824 cve-2024-2756 cve-2024-3096 cve-2024-5458 cve-2024-8925 cve-2024-8926 cve-2024-8927 cve-2024-11236 cve-2024-11234 cve-2024-8932 cve-2024-11233 ghsa-4w77-75f9-2c8w'
      for bugid in $phpseven_bugids; do
        if [[ ! -f "php7234-${bugid}.patch" && -f "$CUR_DIR/patches/php/php7234-${bugid}.patch" ]]; then
          echo
          if [[ "$bugid" != 'php-openssl-cert' ]]; then
            echo "Patching PHP 7.2.34+ for bug #${bugid}"
            echo "https://bugs.php.net/bug.php?id=${bugid}"
          elif [[ "$bugid" = 'sqlite3-defensive' ]]; then
            echo "patching PHP 7.2.34+ sqlite3-defensive patch"
          elif [[ "$bugid" = 'php-openssl-cert' ]]; then
            echo "patching PHP 7.2.34+ php-openssl-cert patch"
          elif [[ "$bugid" = 'mysqlnd-fix' && "$PHP_MYSQLND_PATCH_FIX" = [yY] ]]; then
            echo "patching PHP 7.2.34 backport mysqlnd fix from PHP 7.3 for MariaDB issue https://jira.mariadb.org/browse/MDEV-24121"
            echo
            cp -a $CUR_DIR/patches/php/php7234-${bugid}.patch php7234-${bugid}.patch
            dos2unix php7234-${bugid}.patch
            patch -p1 < php7234-${bugid}.patch
          fi
          if [[ "$bugid" != 'mysqlnd-fix' ]]; then
            echo
            # Copy the patch file
            cp -a $CUR_DIR/patches/php/php7234-${bugid}.patch php7234-${bugid}.patch
            dos2unix php7234-${bugid}.patch
            # Attempt to apply the patch
            if patch -p1 < php7234-${bugid}.patch; then
              echo "Patch applied successfully for bug #${bugid}"
            else
              echo "Patching failed for bug #${bugid}"
              # Check each target file for .orig and .rej files
              # Create a temporary file
              temp_file=$(mktemp)

              # Use grep to find the necessary information and store it in the temporary file
              grep -oP '(?<=\+\+\+ )[^\s]+' "php7234-${bugid}.patch" > "$temp_file"

              # Read from the temporary file
              while read -r file; do
                # Remove the 'b/' prefix from the filename
                file=$(echo "$file" | sed 's/b\///')
                # Check if the original and reject files exist
                if [[ -f "$file.orig" && -f "$file.rej" ]]; then
                  echo "Restoring original file: $file"
                  mv -f "$file.orig" "$file"
                  rm "$file.rej"
                fi
              done < "$temp_file"

              # Remove the temporary file
              rm "$temp_file"
            fi
          fi
        else
          echo "php7234-${bugid}.patch patch file already applied"
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
    if [[ "$PHPMUVER" = '7.3' || "$PHPMVER" = '7.3' ]] && [[ "$PHPVER_ID" -ge '70300' && "$PHPVER_ID" -le '70333' ]]; then
      # backport security bug patch for php 7.3
      phpseven_bugids='81026 81719 81720 pcre1038 81726 81727 81738 81740 81744 81746 cve-2023-0662 ghsa-76gg-c692-v2mw cve-2023-3823 cve-2023-3824 cve-2024-2756 cve-2024-3096 cve-2024-5458 cve-2024-8925 cve-2024-8926 cve-2024-8927 cve-2024-11236 cve-2024-11234 cve-2024-8932 cve-2024-11233 ghsa-4w77-75f9-2c8w'
      for bugid in $phpseven_bugids; do
        if [[ ! -f "php7331-${bugid}.patch" && -f "$CUR_DIR/patches/php/php7331-${bugid}.patch" ]]; then
          echo
          # php7331-pcre1038
          if [[ "$bugid" = 'pcre1038' ]]; then
            cp -a $CUR_DIR/patches/php/php7331-${bugid}.patch php7331-${bugid}.patch
            dos2unix php7331-${bugid}.patch
            patch -p1 < php7331-${bugid}.patch            
          elif [[ "$bugid" != 'php-openssl-cert' ]]; then
            echo "Patching PHP 7.3.31 for bug #${bugid}"
            echo "https://bugs.php.net/bug.php?id=${bugid}"
          fi
          echo
          # Copy the patch file
          cp -a $CUR_DIR/patches/php/php7331-${bugid}.patch php7331-${bugid}.patch
          dos2unix php7331-${bugid}.patch
          # Attempt to apply the patch
          if patch -p1 < php7331-${bugid}.patch; then
            echo "Patch applied successfully for bug #${bugid}"
          else
            echo "Patching failed for bug #${bugid}"
            # Check each target file for .orig and .rej files
            # Create a temporary file
            temp_file=$(mktemp)

            # Use grep to find the necessary information and store it in the temporary file
            grep -oP '(?<=\+\+\+ )[^\s]+' "php7331-${bugid}.patch" > "$temp_file"

            # Read from the temporary file
            while read -r file; do
              # Remove the 'b/' prefix from the filename
              file=$(echo "$file" | sed 's/b\///')
              # Check if the original and reject files exist
              if [[ -f "$file.orig" && -f "$file.rej" ]]; then
                echo "Restoring original file: $file"
                mv -f "$file.orig" "$file"
                rm "$file.rej"
              fi
            done < "$temp_file"

            # Remove the temporary file
            rm "$temp_file"
          fi
        else
          echo "php7331-${bugid}.patch patch file already applied"
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
  if [[ "$PHPVER_ID" -ge '70400' && "$PHPVER_ID" -le '70432' ]]; then
    # for 7.4.0 to 7.4.32 only
    phpseven_bugids='81738 81739'
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
  if [[ "$PHPVER_ID" -ge '70433' && "$PHPVER_ID" -le '70434' ]]; then
      # for 7.4.33 to 7.4.34 only
      phpseven_bugids='81740 81744 81746 cve-2023-0662 ghsa-76gg-c692-v2mw cve-2023-3823 cve-2023-3824 cve-2024-2756 cve-2024-3096 cve-2024-5458 cve-2024-8925 cve-2024-8926 cve-2024-8927 cve-2024-9026 cve-2024-11236 cve-2024-11234 cve-2024-8932 cve-2024-11233 ghsa-4w77-75f9-2c8w'

      for bugid in $phpseven_bugids; do
          if [[ ! -f "php742-${bugid}.patch" && -f "$CUR_DIR/patches/php/php742-${bugid}.patch" ]]; then
              echo
              if [[ "$bugid" != 'php-openssl-cert' ]]; then
                  echo "Patching PHP 7.4.33 for bug #${bugid}"
                  echo "https://bugs.php.net/bug.php?id=${bugid}"
              fi
              echo

              # Copy the patch file
              cp -a $CUR_DIR/patches/php/php742-${bugid}.patch php742-${bugid}.patch
              dos2unix php742-${bugid}.patch

              # Attempt to apply the patch
              if patch -p1 < php742-${bugid}.patch; then
                  echo "Patch applied successfully for bug #${bugid}"
              else
                  echo "Patching failed for bug #${bugid}"

                  # Check each target file for .orig and .rej files
                  # Create a temporary file
                  temp_file=$(mktemp)

                  # Use grep to find the necessary information and store it in the temporary file
                  grep -oP '(?<=\+\+\+ )[^\s]+' "php742-${bugid}.patch" > "$temp_file"

                  # Read from the temporary file
                  while read -r file; do
                    # Remove the 'b/' prefix from the filename
                    file=$(echo "$file" | sed 's/b\///')
                    # Check if the original and reject files exist
                    if [[ -f "$file.orig" && -f "$file.rej" ]]; then
                      echo "Restoring original file: $file"
                      mv -f "$file.orig" "$file"
                      rm "$file.rej"
                    fi
                  done < "$temp_file"

                  # Remove the temporary file
                  rm "$temp_file"
              fi
          else
            echo "php742-${bugid}.patch patch file already applied"
          fi
      done
  fi
  if [[ "$PHPVER_ID" -ge '80000' && "$PHPVER_ID" -le '80099' ]]; then
      # for 8.1.x only
      phpeight_bugids='cve-2024-2756 cve-2024-3096 cve-2024-5458 cve-2024-8925 cve-2024-8926 cve-2024-8927 cve-2024-9026 cve-2024-11236 cve-2024-11234 cve-2024-8932 cve-2024-11233 ghsa-4w77-75f9-2c8w'

      for bugid in $phpeight_bugids; do
          if [[ ! -f "php80-${bugid}.patch" && -f "$CUR_DIR/patches/php/php80-${bugid}.patch" ]]; then
              echo
              if [[ "$bugid" != 'php-openssl-cert' ]]; then
                  echo "Patching PHP 8.0.x for bug #${bugid}"
                  echo "https://bugs.php.net/bug.php?id=${bugid}"
              fi
              echo

              # Copy the patch file
              cp -a $CUR_DIR/patches/php/php80-${bugid}.patch php80-${bugid}.patch
              dos2unix php80-${bugid}.patch

              # Attempt to apply the patch
              if patch -p1 < php80-${bugid}.patch; then
                  echo "Patch applied successfully for bug #${bugid}"
              else
                  echo "Patching failed for bug #${bugid}"

                  # Check each target file for .orig and .rej files
                  # Create a temporary file
                  temp_file=$(mktemp)

                  # Use grep to find the necessary information and store it in the temporary file
                  grep -oP '(?<=\+\+\+ )[^\s]+' "php80-${bugid}.patch" > "$temp_file"

                  # Read from the temporary file
                  while read -r file; do
                    # Remove the 'b/' prefix from the filename
                    file=$(echo "$file" | sed 's/b\///')
                    # Check if the original and reject files exist
                    if [[ -f "$file.orig" && -f "$file.rej" ]]; then
                      echo "Restoring original file: $file"
                      mv -f "$file.orig" "$file"
                      rm "$file.rej"
                    fi
                  done < "$temp_file"

                  # Remove the temporary file
                  rm "$temp_file"
              fi
          else
              echo "php80-${bugid}.patch patch file already applied"
          fi
      done
  fi

  if [[ "$PHPVER_ID" -eq '80226' ]]; then
    phpeight_bugids='revert-gmp'

    for bugid in $phpeight_bugids; do
      if [[ ! -f "php8226-${bugid}.patch" && -f "$CUR_DIR/patches/php/php8226-${bugid}.patch" ]]; then
        echo
        echo "Patching PHP 8.0.x for bug #${bugid}"
        echo "https://bugs.php.net/bug.php?id=${bugid}"
        echo

        # Copy the patch file
        cp -a $CUR_DIR/patches/php/php8226-${bugid}.patch php8226-${bugid}.patch
        dos2unix php8226-${bugid}.patch

        # Attempt to apply the patch
        patch -p1 < php8226-${bugid}.patch
        if [[ $? -ne 0 || $(find . -name "*.rej" | wc -l) -ne 0 ]]; then
          echo "Patching failed for bug #${bugid}"
          
          # Revert changes
          temp_file=$(mktemp)
          grep -oP '(?<=\+\+\+ )[^\s]+' "php8226-${bugid}.patch" > "$temp_file"
          while read -r file; do
            file=$(echo "$file" | sed 's/b\///')
            if [[ -f "$file.orig" && -f "$file.rej" ]]; then
              echo "Restoring original file: $file"
              mv -f "$file.orig" "$file"
              rm "$file.rej"
            fi
          done < "$temp_file"
          rm "$temp_file"
        else
          echo "Patch applied successfully for bug #${bugid}"
        fi
      else
        echo "php8226-${bugid}.patch patch file already applied"
      fi
    done
  fi

  if [[ "$PHPVER_ID" -eq '80314' ]]; then
    phpeight_bugids='revert-gmp'

    for bugid in $phpeight_bugids; do
      if [[ ! -f "php8314-${bugid}.patch" && -f "$CUR_DIR/patches/php/php8314-${bugid}.patch" ]]; then
        echo
        echo "Patching PHP 8.0.x for bug #${bugid}"
        echo "https://bugs.php.net/bug.php?id=${bugid}"
        echo

        # Copy the patch file
        cp -a $CUR_DIR/patches/php/php8314-${bugid}.patch php8314-${bugid}.patch
        dos2unix php8314-${bugid}.patch

        # Attempt to apply the patch
        patch -p1 < php8314-${bugid}.patch
        if [[ $? -ne 0 || $(find . -name "*.rej" | wc -l) -ne 0 ]]; then
          echo "Patching failed for bug #${bugid}"
          
          # Revert changes
          temp_file=$(mktemp)
          grep -oP '(?<=\+\+\+ )[^\s]+' "php8314-${bugid}.patch" > "$temp_file"
          while read -r file; do
            file=$(echo "$file" | sed 's/b\///')
            if [[ -f "$file.orig" && -f "$file.rej" ]]; then
              echo "Restoring original file: $file"
              mv -f "$file.orig" "$file"
              rm "$file.rej"
            fi
          done < "$temp_file"
          rm "$temp_file"
        else
          echo "Patch applied successfully for bug #${bugid}"
        fi
      else
        echo "php8314-${bugid}.patch patch file already applied"
      fi
    done
  fi

  if [[ "$PHPVER_ID" -eq '80401' ]]; then
    phpeight_bugids='revert-gmp'

    for bugid in $phpeight_bugids; do
      if [[ ! -f "php841-${bugid}.patch" && -f "$CUR_DIR/patches/php/php841-${bugid}.patch" ]]; then
        echo
        echo "Patching PHP 8.0.x for bug #${bugid}"
        echo "https://bugs.php.net/bug.php?id=${bugid}"
        echo

        # Copy the patch file
        cp -a $CUR_DIR/patches/php/php841-${bugid}.patch php841-${bugid}.patch
        dos2unix php841-${bugid}.patch

        # Attempt to apply the patch
        patch -p1 < php841-${bugid}.patch
        if [[ $? -ne 0 || $(find . -name "*.rej" | wc -l) -ne 0 ]]; then
          echo "Patching failed for bug #${bugid}"
          
          # Revert changes
          temp_file=$(mktemp)
          grep -oP '(?<=\+\+\+ )[^\s]+' "php841-${bugid}.patch" > "$temp_file"
          while read -r file; do
            file=$(echo "$file" | sed 's/b\///')
            if [[ -f "$file.orig" && -f "$file.rej" ]]; then
              echo "Restoring original file: $file"
              mv -f "$file.orig" "$file"
              rm "$file.rej"
            fi
          done < "$temp_file"
          rm "$temp_file"
        else
          echo "Patch applied successfully for bug #${bugid}"
        fi
      else
        echo "php841-${bugid}.patch patch file already applied"
      fi
    done
  fi

  # Check if the system is CentOS 7 and PHP version is 8.3.16 or higher
  if [[ "$CENTOS_SEVEN" -eq '7' && "$PHPVER_ID" -ge '80316' ]]; then
    # List of bug IDs to patch (add more if needed)
    phpseven_ffi_bugids='ffi-c7'

    for bugid in $phpseven_ffi_bugids; do
      # Define paths for the patch file
      patch_source="$CUR_DIR/patches/php/php8316-${bugid}.patch"
      patch_target="php8316-${bugid}.patch"

      # Verify that the patch file exists
      if [[ ! -f "$patch_source" ]]; then
        echo "Patch file $patch_source does not exist. Skipping patch for bug #${bugid}."
        continue
      fi

      # Define the target file to check for the patch's unique identifier
      target_file="${DIR_TMP}/php-8.3.16/ext/ffi/ffi.c"

      # Check if the patch has already been applied by searching for the unique macro
      if grep -q "ZEND_STRNCMP" "$target_file"; then
        echo "Patch php8316-${bugid}.patch has already been applied. Skipping."
        continue
      fi

      echo
      echo "Applying patch php8316-${bugid}.patch for CentOS 7 with glibc 2.17"
      echo "Related Issue: https://github.com/php/php-src/issues/17498"
      echo

      # Copy the patch file to the PHP source directory
      cp -a "$patch_source" "$patch_target"
      
      # Convert Windows line endings to Unix if necessary
      dos2unix "$patch_target" > /dev/null 2>&1

      # Attempt to apply the patch with the -N flag to ignore already applied patches
      patch -N -p1 < "$patch_target"

      # Check if the patch was applied successfully
      if [[ $? -eq 0 ]]; then
        echo "Patch php8316-${bugid}.patch applied successfully."
      else
        echo "Failed to apply patch php8316-${bugid}.patch for bug #${bugid}."
        
        # Check for any rejected hunks and attempt to revert changes
        if [[ -f "*.rej" ]]; then
          echo "Reverting changes due to patch failure..."
          
          # Revert using the original file backup if available
          # Note: This assumes that patch created a .orig file; adjust if different
          for file in *.orig; do
            if [[ -f "$file" ]]; then
              original_file="${file%.orig}"
              echo "Restoring original file: $original_file"
              mv -f "$file" "$original_file"
            fi
          done
          
          # Remove any .rej files
          rm -f *.rej
        fi

        # Optionally, exit the script or continue based on your preference
        # exit 1
      fi

      # Clean up the copied patch file
      rm -f "$patch_target"
    done
  else
    echo "System is not CentOS 7 or PHP version is below 8.3.16. No patches applied."
  fi

  # backport PHP 8.0 OpenSSL 3 compatibility for PHP 7.4
  # only compilation fixes but not 100% working as extensive changes
  # needed for 7.4 & 8.0 for compatibility
  # "$PHP_PATCH_OPENSSL_THREE" = [yY]
  if [[ "$CENTOS_EIGHT" -eq '8' && "$PHP_PATCH_OPENSSL_THREE" = [yY] ]] && [[ "$PHPVER_ID" -ge '70400' && "$PHPVER_ID" -lt '80000' ]]; then
    phpsevenfour_opensslthree_bugids='php74-snmp-openssl3 php74-openssl3'
    for bugid in $phpsevenfour_opensslthree_bugids; do
      echo "EL8 PHP 7.4 $bugid"
      ls -lAh "$CUR_DIR/patches/php/${bugid}.patch"
      if [[ ! -f "${bugid}.patch" && -f "$CUR_DIR/patches/php/${bugid}.patch" ]]; then
        echo
        if [[ "$bugid" != 'php-openssl-cert' ]]; then
          echo "patching PHP 7.4 for ${bugid}"
        fi
        echo
        cp -a $CUR_DIR/patches/php/${bugid}.patch ${bugid}.patch
        dos2unix ${bugid}.patch
        patch -p1 < ${bugid}.patch
      fi
    done
  fi
  if [[ "$CENTOS_NINE" -eq '9' && "$PHP_PATCH_OPENSSL_THREE" = [yY] ]] && [[ "$PHPVER_ID" -ge '70400' && "$PHPVER_ID" -lt '80000' ]]; then
    phpsevenfour_opensslthree_bugids='php74-snmp-openssl3 php74-openssl3'
    for bugid in $phpsevenfour_opensslthree_bugids; do
      echo "EL9 PHP 7.4 $bugid"
      ls -lAh "$CUR_DIR/patches/php/${bugid}.patch"
      if [[ ! -f "${bugid}.patch" && -f "$CUR_DIR/patches/php/${bugid}.patch" ]]; then
        echo
        if [[ "$bugid" != 'php-openssl-cert' ]]; then
          echo "patching PHP 7.4 for ${bugid}"
        fi
        echo
        cp -a $CUR_DIR/patches/php/${bugid}.patch ${bugid}.patch
        dos2unix ${bugid}.patch
        patch -p1 < ${bugid}.patch
      fi
    done
  fi
  # if [[ "$CENTOS_NINE" -eq '9' ]] && [[ "$PHPVER_ID" -ge '70400' && "$PHPVER_ID" -lt '80000' ]]; then
  #   phpsevenfour_opensslthree_bugids='php74-snmp-openssl3'
  #   for bugid in $phpsevenfour_opensslthree_bugids; do
  #     if [[ ! -f "${bugid}.patch" && -f "$CUR_DIR/patches/php/${bugid}.patch" ]]; then
  #       echo
  #       if [[ "$bugid" != 'php-openssl-cert' ]]; then
  #         echo "patching PHP 7.4 for ${bugid}"
  #       fi
  #       echo
  #       cp -a $CUR_DIR/patches/php/${bugid}.patch ${bugid}.patch
  #       dos2unix ${bugid}.patch
  #       patch -p1 < ${bugid}.patch
  #     fi
  #   done
  # fi
  # if [[ "$CENTOS_NINE" -eq '9' ]] && [[ "$PHPVER_ID" -ge '80000' && "$PHPVER_ID" -lt '80100' ]]; then
  #   # phar: switch to sha256 signature by default, backported from 8.1
  #   # phar: implement openssl_256 and openssl_512 for signatures, backported from 8.1
  #   # snmp: add sha256 / sha512 security protocol, backported from 8.1
  #   phpeightzero_opensslthree_bugids='php80-openssl3-phar-sha'
  #   for bugid in $phpeightzero_opensslthree_bugids; do
  #     if [[ ! -f "${bugid}.patch" && -f "$CUR_DIR/patches/php/${bugid}.patch" ]]; then
  #       echo
  #       if [[ "$bugid" != 'php-openssl-cert' ]]; then
  #         echo "patching PHP 8.0 for ${bugid}"
  #       fi
  #       echo
  #       cp -a $CUR_DIR/patches/php/${bugid}.patch ${bugid}.patch
  #       dos2unix ${bugid}.patch
  #       patch -p1 < ${bugid}.patch
  #     fi
  #   done
  # fi
  # backport PHP 8.0 OpenSSL 3 compatibility for PHP 8.0
  # only compilation fixes but not 100% working as extensive changes
  # needed for 7.4 & 8.0 for compatibility
  # "$PHP_PATCH_OPENSSL_THREE" = [yY]
  if [[ "$CENTOS_EIGHT" -eq '8' && "$PHP_PATCH_OPENSSL_THREE" = [yY] ]] && [[ "$PHPVER_ID" -ge '80000' && "$PHPVER_ID" -lt '80100' ]]; then
    phpeightzero_opensslthree_bugids='php80-snmp-openssl3 php80-openssl3-phar-sha php80-openssl3'
    for bugid in $phpeightzero_opensslthree_bugids; do
      echo "EL8 PHP 8.0 $bugid"
      ls -lAh "$CUR_DIR/patches/php/${bugid}.patch"
      if [[ ! -f "${bugid}.patch" && -f "$CUR_DIR/patches/php/${bugid}.patch" ]]; then
        echo
        if [[ "$bugid" != 'php-openssl-cert' ]]; then
          echo "patching PHP 8.0 for ${bugid}"
        fi
        echo
        cp -a $CUR_DIR/patches/php/${bugid}.patch ${bugid}.patch
        dos2unix ${bugid}.patch
        patch -p1 < ${bugid}.patch
      fi
    done
  fi
  if [[ "$CENTOS_NINE" -eq '9' && "$PHP_PATCH_OPENSSL_THREE" = [yY] ]] && [[ "$PHPVER_ID" -ge '80000' && "$PHPVER_ID" -lt '80100' ]]; then
    phpeightzero_opensslthree_bugids='php80-snmp-openssl3 php80-openssl3-phar-sha php80-openssl3'
    for bugid in $phpeightzero_opensslthree_bugids; do
      echo "EL9 PHP 8.0 $bugid"
      ls -lAh "$CUR_DIR/patches/php/${bugid}.patch"
      if [[ ! -f "${bugid}.patch" && -f "$CUR_DIR/patches/php/${bugid}.patch" ]]; then
        echo
        if [[ "$bugid" != 'php-openssl-cert' ]]; then
          echo "patching PHP 8.0 for ${bugid}"
        fi
        echo
        cp -a $CUR_DIR/patches/php/${bugid}.patch ${bugid}.patch
        dos2unix ${bugid}.patch
        patch -p1 < ${bugid}.patch
      fi
    done
  fi
  if [[ "$PHPVER_ID" -lt '80100' && "$FPM_MAX_SPAWN_RATE" -ge '32' && -f sapi/fpm/fpm/fpm_process_ctl.h ]]; then
    echo
    echo "adjust FPM_MAX_SPAWN_RATE for PHP-FPM"
    sed -i "s|FPM_MAX_SPAWN_RATE .*|FPM_MAX_SPAWN_RATE ($FPM_MAX_SPAWN_RATE)|" sapi/fpm/fpm/fpm_process_ctl.h
    grep -in 'FPM_MAX_SPAWN_RATE' sapi/fpm/fpm/fpm_process_ctl.h
    echo
  fi
} 2>&1 | tee -a "${CENTMINLOGDIR}/patch_php_${DT}.log"
}