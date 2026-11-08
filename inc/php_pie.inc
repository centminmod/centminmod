########################################################################
# PHP PIE (PHP Installer for Extensions) https://github.com/php/pie
# PHP_PIE_INSTALL='y' builds supported PHP extensions with a pinned,
# sha256 verified pie.phar instead of PECL tarballs. php_pie_install
# returns non-zero whenever PIE cannot be used or its build is not
# verified, and every caller then runs its existing PECL build.
# PIE never writes ini files here (--skip-enable-extension); the
# extension installers write ${CONFIGSCANDIR}/<ext>.ini as before.
# Extension sources are Packagist/GitHub zipballs without checksums, so
# like the PECL tarballs they are trusted through TLS only.
# This file only defines functions.

# Once per PHP build: check this PHP can run PIE and that the pinned pie.phar
# is installed and matches PHP_PIE_SHA256, downloading it when needed.
php_pie_setup() {
  local sum f phpid dl_dir="${DIR_TMP}/pie-download"
  phpid=$(/usr/local/bin/php-config --vernum 2>/dev/null)
  # cached per PHP build, a menu session can switch PHP versions
  if [[ -n "$PHP_PIE_READY" && "$PHP_PIE_READY_ID" = "$phpid" ]]; then
    [[ "$PHP_PIE_READY" = 'y' ]]
    return
  fi
  PHP_PIE_READY='n'
  PHP_PIE_READY_ID="$phpid"
  if [[ ! "$PHP_PIE_VER" =~ ^[0-9]+\.[0-9]+\.[0-9]+$ || ! "$PHP_PIE_SHA256" =~ ^[0-9a-f]{64}$ ]]; then
    cecho "PIE: PHP_PIE_VER and PHP_PIE_SHA256 must name a PIE release and its pie.phar sha256, using PECL builds" $boldyellow
    return 1
  fi
  PHP_PIE_PHAR="/usr/local/lib/pie/pie-${PHP_PIE_VER}.phar"
  if [[ ! -x /usr/local/bin/php || ! -x /usr/local/bin/phpize || ! -x /usr/local/bin/php-config ]]; then
    cecho "PIE: /usr/local/bin/php, phpize or php-config not found, using PECL builds" $boldyellow
    return 1
  fi
  if [[ "${phpid:-0}" -lt 80100 ]]; then
    cecho "PIE: needs PHP 8.1 or newer to run, PHP is $(/usr/local/bin/php-config --version), using PECL builds" $boldyellow
    return 1
  fi
  if ! /usr/local/bin/php -m 2>/dev/null | grep -qix zip && ! command -v unzip >/dev/null 2>&1; then
    cecho "PIE: needs the PHP zip extension or unzip, using PECL builds" $boldyellow
    return 1
  fi
  sum=$(sha256sum "$PHP_PIE_PHAR" 2>/dev/null | awk '{print $1}')
  if [[ "$sum" != "$PHP_PIE_SHA256" ]]; then
    cecho "PIE: downloading pie.phar ${PHP_PIE_VER}" $boldgreen
    rm -rf "$dl_dir"
    if ! mkdir -p "$dl_dir" /usr/local/lib/pie; then
      cecho "PIE: cannot create ${dl_dir} or /usr/local/lib/pie, using PECL builds" $boldyellow
      return 1
    fi
    # bounded, unlike download_cmd: a blocked github.com must not stall every menu run
    curl -${ipv_forceopt}fsSL --connect-timeout 20 --max-time 300 --retry 2 --retry-max-time 300 \
      -o "${dl_dir}/pie.phar" "https://github.com/php/pie/releases/download/${PHP_PIE_VER}/pie.phar"
    sum=$(sha256sum "${dl_dir}/pie.phar" 2>/dev/null | awk '{print $1}')
    if [[ "$sum" != "$PHP_PIE_SHA256" ]]; then
      rm -rf "$dl_dir"
      cecho "PIE: pie.phar ${PHP_PIE_VER} download failed or sha256 mismatch (got ${sum:-no file}), using PECL builds" $boldyellow
      return 1
    fi
    if ! install -m 0644 "${dl_dir}/pie.phar" "$PHP_PIE_PHAR"; then
      rm -rf "$dl_dir"
      cecho "PIE: cannot install ${PHP_PIE_PHAR}, using PECL builds" $boldyellow
      return 1
    fi
    rm -rf "$dl_dir"
    for f in /usr/local/lib/pie/pie-*.phar; do
      [[ "$f" = "$PHP_PIE_PHAR" ]] || rm -f "$f"
    done
  fi
  if ! /usr/local/bin/php "$PHP_PIE_PHAR" --version </dev/null 2>&1 | grep -qF "$PHP_PIE_VER"; then
    cecho "PIE: pie.phar ${PHP_PIE_VER} does not run with PHP $(/usr/local/bin/php-config --version), using PECL builds" $boldyellow
    return 1
  fi
  PHP_PIE_READY='y'
}

# Build and install one extension with PIE.
# $1 = extension (module name, <ext>.so and <ext>.ini), $2 = vendor/package:version,
# then optional VAR=value build environment entries, then -- and the configure
# options the package declares (php pie.phar info vendor/package lists them).
# Returns 0 only when PIE installed a new <ext>.so that loads with the current
# ${CONFIGSCANDIR} ini files and reports the requested version. The caller
# writes <ext>.ini. Any other outcome puts the previous <ext>.so back (or
# removes PIE's new one), returns 1, and the caller builds from PECL.
php_pie_install() {
  local ext="$1" pkg="$2" ver="${2##*:}" envs=() status=0 reason='' extdir so bak log marker check loaded
  shift 2
  while [[ $# -gt 0 && "$1" != '--' ]]; do
    envs+=("$1")
    shift
  done
  [[ "$1" = '--' ]] && shift
  php_pie_setup || return 1
  extdir=$(/usr/local/bin/php-config --extension-dir 2>/dev/null)
  if [[ "$extdir" != /* || ! -d "$extdir" ]]; then
    cecho "PIE: PHP extension directory '${extdir}' not found, using the PECL build instead" $boldyellow
    return 1
  fi
  so="${extdir}/${ext}.so"
  bak="${so}.pie-bak"
  envs+=("HOME=${HOME:-/root}" "PIE_WORKING_DIRECTORY=${DIR_TMP}/pie-work" 'COMPOSER_NO_INTERACTION=1')
  [[ "$FORCE_IPVFOUR" = [yY] ]] && envs+=('COMPOSER_IPRESOLVE=4')
  log="${CENTMINLOGDIR}/php-pie-${ext}_${DT}.log"
  marker="${DIR_TMP}/pie-start-${ext}"
  # a fresh working dir means PIE never skips a build as "already installed";
  # PIE's make install replaces the live .so, so keep a copy to put back on failure
  rm -rf "${DIR_TMP}/pie-work" "$bak"
  if ! touch "$marker" || { [[ -f "$so" ]] && ! cp -p "$so" "$bak"; }; then
    rm -f "$marker" "$bak"
    cecho "PIE: cannot write ${marker} or ${bak}, using the PECL build instead" $boldyellow
    return 1
  fi
  cecho "PIE: installing ${pkg} for PHP $(/usr/local/bin/php-config --version)" $boldgreen
  echo "pie install ${pkg} $*"
  (
    export "${envs[@]}"
    exec /usr/local/bin/php "$PHP_PIE_PHAR" install -n -v \
      --with-php-config=/usr/local/bin/php-config \
      --with-phpize-path=/usr/local/bin/phpize \
      --skip-enable-extension \
      --no-build-tools-check \
      --no-system-dependencies-check \
      --suppress-download-url-method=pre-packaged-binary \
      --make-parallel-jobs="$CPUS" \
      "$pkg" "$@"
  ) </dev/null 2>&1 | tee -a "$log"
  status=${PIPESTATUS[0]}
  if [[ "$status" -ne 0 ]]; then
    reason="failed (status ${status}), log: ${log}"
  elif [[ ! -s "$so" || ! "$so" -nt "$marker" ]]; then
    reason="did not install a new ${so}"
  else
    # load test with today's ini files plus <ext>.ini, before Centmin writes <ext>.ini
    check="${DIR_TMP}/pie-inicheck"
    rm -rf "$check"
    if mkdir -p "$check"; then
      cp -p "${CONFIGSCANDIR}"/*.ini "$check"/ 2>/dev/null
      echo "extension=${ext}.so" > "${check}/${ext}.ini"
      loaded=$(PHP_INI_SCAN_DIR="$check" /usr/local/bin/php -r "echo phpversion('${ext}');" 2>/dev/null)
      rm -rf "$check"
    fi
    [[ "$loaded" = "$ver" ]] || reason="built ${ext}.so reports version '${loaded:-not loaded}', expected ${ver}"
  fi
  if [[ -n "$reason" ]]; then
    if [[ -f "$bak" ]]; then
      mv -f "$bak" "$so"
    elif [[ "$so" -nt "$marker" ]]; then
      rm -f "$so"
    fi
    rm -f "$marker"
    cecho "PIE: ${pkg} ${reason}, using the PECL build instead" $boldyellow
    if [[ "$status" -ne 0 ]] && grep -qi 'rate limit' "$log"; then
      cecho "PIE: GitHub API rate limit reached; a read-only GitHub token in Composer's auth.json raises it, see PHP_PIE_INSTALL in example/custom_config.inc" $boldyellow
    fi
    return 1
  fi
  rm -f "$marker" "$bak"
  cecho "PIE: ${ext} ${ver} installed in ${extdir}" $boldgreen
  return 0
}
