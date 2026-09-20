#!/bin/bash
# Isolated checks of the real version gates; no packages or services changed.
set -e
repo=$(cd "$(dirname "$0")/.." && pwd)
scratch=$(mktemp -d "$repo/.openssl-test.XXXXXX")
trap 'rm -rf "$scratch"' EXIT
ktls=$(awk '/^# Classify the library selected/ {copy=1} /^# openssl 1.1.1 quictls fork/ {exit} copy {print}' "$repo/inc/nginx_configure.inc")
[[ -n "$ktls" ]]
check_ktls() { eval "$ktls"; }
NGINX_KTLS=y KERNEL_NUMERICVER=6000000000 NGINX_QUIC_SUPPORT=n ngver=1.31.5
LIBRESSL_SWITCH=n BORINGSSL_SWITCH=n AWS_LC_SWITCH=n OPENSSL_SYSTEM_USE=n
for version in 1.1.1w 3.0.19 3.5.8 3.6.4 4.0.2 4.1.0-alpha1; do
  OPENSSL_VERSION=$version
  check_ktls >/dev/null
  case "$version" in
    3.*|4.0.*) [[ "$NGINX_KTLS_OPT" = ' enable-ktls' ]];;
    *) [[ -z "$NGINX_KTLS_OPT" ]];;
  esac
done
OPENSSL_SYSTEM_USE=y
for OPENSSL_VERSION in 3.6.4 4.0.2; do
  check_ktls >/dev/null
  [[ -z "$NGINX_KTLS_OPT" ]]
done
OPENSSL_SYSTEM_USE=n
OPENSSL_VERSION=4.0.2 NGINX_QUIC_SUPPORT=y OPENSSL_QUIC_VERSION=1.1.1w
check_ktls >/dev/null
[[ -z "$NGINX_KTLS_OPT" ]]
OPENSSL_QUIC_VERSION=3.3.0
check_ktls >/dev/null
[[ "$NGINX_KTLS_OPT" = ' enable-ktls' ]]
for alternate in LIBRESSL_SWITCH BORINGSSL_SWITCH AWS_LC_SWITCH; do
  printf -v "$alternate" '%s' y
  check_ktls >/dev/null
  [[ -z "$NGINX_KTLS_OPT" ]]
  printf -v "$alternate" '%s' n
done
NGINX_QUIC_SUPPORT=n KERNEL_NUMERICVER=4018000000
check_ktls >/dev/null
[[ -z "$NGINX_KTLS_OPT" ]]
KERNEL_NUMERICVER=6000000000 NGINX_KTLS=n
check_ktls >/dev/null
[[ -z "$NGINX_KTLS_OPT" ]]

minimum=$(sed -n '/^  # Upstream added OpenSSL 4.0 API compatibility/,/^  fi$/p' "$repo/inc/nginx_configure.inc")
[[ -n "$minimum" ]]
check_minimum() { eval "$minimum"; }
nginx_openssl_version=4.0.2 OPENSSL_SYSTEM_USE=n LIBRESSL_SWITCH=n BORINGSSL_SWITCH=n AWS_LC_SWITCH=n
DETECT_NGXVER=1029007
if check_minimum 2>/dev/null; then echo 'FAIL: old Nginx accepted for OpenSSL 4.0'; exit 1; fi
DETECT_NGXVER=1029008
check_minimum
DETECT_NGXVER=1031005
check_minimum
DETECT_NGXVER=1028000 nginx_openssl_version=3.6.4
check_minimum

abi=$(awk '/# A major-version replacement must not remove/ {copy=1} copy && /rm -rf "\$STATICLIBSSL"/ {exit} copy {print}' "$repo/inc/openssl_install.inc")
[[ -n "$abi" ]]
abi=${abi//\/usr\/local\/bin\/php-cgi/$scratch/php-cgi}
abi=${abi//\/usr\/local\/bin\/php/$scratch/php}
abi=${abi//\/usr\/local\/sbin\/php-fpm/$scratch/php-fpm}
abi=${abi//\/usr\/local\/sbin\/nginx/$scratch/nginx}
printf '#!/bin/bash\nexit 0\n' > "$scratch/php"
chmod +x "$scratch/php"
check_abi() { eval "$abi"; }
ldd() { echo "$linked_library"; }
STATICLIBSSL=/opt/openssl OPENSSL_VERSION=4.0.2
for soname in 1.1 3; do
  linked_library="libssl.so.$soname => /opt/openssl/lib64/libssl.so.$soname (0x1234)"
  if (check_abi) 2>/dev/null; then echo 'FAIL: live older ABI replacement accepted'; exit 1; fi
done
linked_library='libssl.so.3 => /usr/lib64/libssl.so.3 (0x1234)'
check_abi
linked_library='libssl.so.4 => /opt/openssl/lib64/libssl.so.4 (0x1234)'
check_abi
OPENSSL_VERSION=3.6.4 linked_library='libssl.so.3 => /opt/openssl/lib64/libssl.so.3 (0x1234)'
check_abi
# Diagnostic update checks must query the installed modern release branch.
eval "$(sed -n '/^version_to_number() {/,/^}/p' "$repo/tools/nginx_crypto_check.sh")"
eval "$(sed -n '/^check_version() {/,/^}/p' "$repo/tools/nginx_crypto_check.sh")"
get_latest_openssl_version() { [[ "$1" = "$expected_branch" ]] || return 1; echo "$1.9"; }
NGINX_CRYPTO_LIBRARY_USED=OpenSSL
for expected_branch in 3.6 4.0; do
  NGINX_CRYPTO_LIBRARY_VERSION="$expected_branch.1"
  [[ "$(check_version)" = *"OpenSSL $expected_branch is available: $expected_branch.9"* ]]
done
echo 'PASS: OpenSSL version/reporting/kTLS gates, QuicTLS separation, Nginx minimum, shared ABI protection'
