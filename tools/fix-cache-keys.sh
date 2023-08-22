#!/bin/bash
#################################################################
# fix to ensure $http_accept_encoding is included in cache keys
#################################################################
TARGET_DIR1="/usr/local/nginx/conf/wpincludes"
TARGET_DIR2="/usr/local/nginx/conf"
SEARCH_FILE1="php-fastcgicache.conf"
SEARCH_FILE2="php-rediscache.conf"

MODE=$1

if [ -d "$TARGET_DIR1" ]; then
  find "$TARGET_DIR1" -type f -name "$SEARCH_FILE1" | while read -r file; do
      if [[ "$MODE" != 'silent' ]]; then
        echo "Processing $file..."
      fi
      sed -Ei 's#fastcgi_cache_key[[:space:]]+\$scheme\$request_method\$host\$request_uri;#fastcgi_cache_key "$scheme$request_method$host$request_uri$http_accept_encoding";#' "$file"
      sed -Ei 's#fastcgi_cache_key[[:space:]]+\$scheme\$request_method\$host\$request_uri\$wpfcgi_wosession;#fastcgi_cache_key "$scheme$request_method$host$request_uri$http_accept_encoding$wpfcgi_wosession";#' "$file"
      if [[ "$MODE" != 'silent' ]]; then
        echo "$file processed."
        echo "-------------------------"
      fi
  done
fi

if [ -f "${TARGET_DIR2}/${SEARCH_FILE2}" ]; then
  find "$TARGET_DIR2" -type f -name "$SEARCH_FILE2" | while read -r file; do
      if [[ "$MODE" != 'silent' ]]; then
        echo "Processing $file..."
      fi
      sed -Ei 's#set[[:space:]]+\$key[[:space:]]+"nginx-cache:\$scheme\$request_method\$host\$request_uri";#set $key "nginx-cache:$scheme$request_method$host$request_uri$http_accept_encoding";#' "$file"   
      if [[ "$MODE" != 'silent' ]]; then
        echo "$file processed."
        echo "-------------------------"
      fi
  done
fi
if [[ "$MODE" != 'silent' ]]; then
  echo "All files processed."
fi
