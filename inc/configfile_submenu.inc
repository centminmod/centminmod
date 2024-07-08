persistentconfig_submenu() {
  PERSISTENT_CONFIG_FILE="/etc/centminmod/custom_config.inc"

  backup_persistent_config_file() {
    if [ -f "$PERSISTENT_CONFIG_FILE" ]; then
      DT=$(date +"%d%m%y-%H%M%S")
      cp -a "$PERSISTENT_CONFIG_FILE" "${PERSISTENT_CONFIG_FILE}-${DT}"
    fi
  }

  append_persistent_config() {
    local option="$1"
    local value="$2"
    local log_file="$3"

    if grep -q "^${option}=" "$PERSISTENT_CONFIG_FILE"; then
      sed -i "s|^${option}=.*|# & - $(date)|" "$PERSISTENT_CONFIG_FILE"
    fi

    echo "${option}='${value}'" >> "$PERSISTENT_CONFIG_FILE"
    echo "$(date): ${option} set to '${value}'" >> "$log_file"
  }

  while :
  do
    echo
    cecho "---------------------------------------------------------------------" $boldyellow
    cecho "     Centmin Mod Persistent Config File Override Options" $boldgreen
    cecho "---------------------------------------------------------------------" $boldyellow
    cecho "1).  List Persistent Config File options" $boldgreen
    cecho "2).  Enable Letsencrypt SSL support LETSENCRYPT_DETECT='y'" $boldgreen
    cecho "3).  Enable dual SSL RSA+ECDSA certificate support DUALCERTS='y'" $boldgreen
    cecho "4).  Display PHP-FPM version checks on SSH login DMOTD_PHPCHECK='y'" $boldgreen
    cecho "5).  Switch Nginx from OpenSSL to AWS-LC AWS_LC_SWITCH='y'" $boldgreen
    cecho "6).  Enable Nginx Brotli support NGINX_LIBBROTLI='y' NGXDYNAMIC_BROTLI='y'" $boldgreen
    cecho "7).  Enable Nginx zstd support NGINX_ZSTD='y'" $boldgreen
    cecho "8).  Enable Nginx mold linker compile support NGX_LDMOLD='y'" $boldgreen
    cecho "9).  Enable Nginx pcre2 support NGINX_PCRE_TWO='y'" $boldgreen
    cecho "10). Enable Nginx njs module support NGINX_NJS='y' NGXDYNAMIC_NJS='y'" $boldgreen
    cecho "11). Enable Nginx Video modules (Slice, RTMP, FLV, MP4) NGINX_VIDEO='y'" $boldgreen
    cecho "12). Enable PHP-FPM mold linker compile support PHP_LDMOLD='y'" $boldgreen
    cecho "---------------------------------------------------------------------" $boldyellow
    cecho "13). Disable Letsencrypt SSL support LETSENCRYPT_DETECT='n'" $boldgreen
    cecho "14). Disable dual SSL RSA+ECDSA certificate support  DUALCERTS='n'" $boldgreen
    cecho "15). Disable PHP-FPM version checks on SSH login DMOTD_PHPCHECK='n'" $boldgreen
    cecho "16). Switch Nginx back to OpenSSL from AWS-LC AWS_LC_SWITCH='n'" $boldgreen
    cecho "17). Disable Nginx Brotli support NGINX_LIBBROTLI='n' NGXDYNAMIC_BROTLI='n'" $boldgreen
    cecho "18). Disable Nginx zstd support NGINX_ZSTD='n'" $boldgreen
    cecho "19). Disable Nginx mold linker compile support NGX_LDMOLD='n'" $boldgreen
    cecho "20). Disable Nginx pcre2 support NGINX_PCRE_TWO='n'" $boldgreen
    cecho "21). Disable Nginx njs module support NGINX_NJS='n' NGXDYNAMIC_NJS='n'" $boldgreen
    cecho "22). Disable Nginx Video modules (Slice, RTMP, FLV, MP4) NGINX_VIDEO='n'" $boldgreen
    cecho "23). Disable PHP-FPM mold linker compile support PHP_LDMOLD='n'" $boldgreen
    cecho "24). Back to Main menu" $boldgreen
    cecho "---------------------------------------------------------------------" $boldyellow

    read -ep "Enter option [ 1 - 24 ] " configoption
    cecho "---------------------------------------------------------------------" $boldyellow

    DT=$(date +"%d%m%y-%H%M%S")
    log_file="${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_menuoption_${configoption}_${DT}.log"

    case "$configoption" in
      1)
        cecho "Listing Persistent Config File options:" $boldgreen
        cat "$PERSISTENT_CONFIG_FILE"
        ;;
      2)
        backup_persistent_config_file
        append_persistent_config "LETSENCRYPT_DETECT" "y" "$log_file"
        cecho "Letsencrypt SSL support enabled" $boldgreen
        ;;
      3)
        backup_persistent_config_file
        append_persistent_config "DUALCERTS" "y" "$log_file"
        if ! grep -q "^LETSENCRYPT_DETECT=" "$PERSISTENT_CONFIG_FILE"; then
          append_persistent_config "LETSENCRYPT_DETECT" "y" "$log_file"
        fi
        cecho "Dual SSL RSA+ECDSA certificate support enabled" $boldgreen
        ;;
      4)
        backup_persistent_config_file
        append_persistent_config "DMOTD_PHPCHECK" "y" "$log_file"
        cecho "PHP-FPM version checks on SSH login enabled" $boldgreen
        ;;
      5)
        backup_persistent_config_file
        append_persistent_config "AWS_LC_SWITCH" "y" "$log_file"
        cecho "Nginx switched to AWS-LC crypto library" $boldgreen
        ;;
      6)
        backup_persistent_config_file
        append_persistent_config "NGXDYNAMIC_BROTLI" "y" "$log_file"
        append_persistent_config "NGINX_LIBBROTLI" "y" "$log_file"
        cecho "Nginx Brotli support enabled" $boldgreen
        ;;
      7)
        backup_persistent_config_file
        append_persistent_config "NGINX_ZSTD" "y" "$log_file"
        cecho "Nginx zstd support enabled" $boldgreen
        ;;
      8)
        backup_persistent_config_file
        append_persistent_config "NGX_LDMOLD" "y" "$log_file"
        cecho "Nginx mold linker compile support enabled" $boldgreen
        ;;
      9)
        backup_persistent_config_file
        append_persistent_config "NGINX_PCRE_TWO" "y" "$log_file"
        cecho "Nginx pcre2 support enabled" $boldgreen
        ;;
      10)
        backup_persistent_config_file
        append_persistent_config "NGINX_NJS" "y" "$log_file"
        append_persistent_config "NGXDYNAMIC_NJS" "y" "$log_file"
        cecho "Nginx njs module support enabled" $boldgreen
        ;;
      11)
        backup_persistent_config_file
        append_persistent_config "NGINX_VIDEO" "y" "$log_file"
        cecho "Nginx Video modules enabled (Slice, RTMP, FLV, MP4)" $boldgreen
        ;;
      12)
        backup_persistent_config_file
        append_persistent_config "PHP_LDMOLD" "y" "$log_file"
        cecho "PHP-FPM mold linker compile support enabled" $boldgreen
        ;;
      13)
        backup_persistent_config_file
        append_persistent_config "LETSENCRYPT_DETECT" "n" "$log_file"
        cecho "Letsencrypt SSL support disabled" $boldgreen
        ;;
      14)
        backup_persistent_config_file
        append_persistent_config "DUALCERTS" "n" "$log_file"
        cecho "Dual SSL RSA+ECDSA certificate support disabled" $boldgreen
        ;;
      15)
        backup_persistent_config_file
        append_persistent_config "DMOTD_PHPCHECK" "n" "$log_file"
        cecho "PHP-FPM version checks on SSH login disabled" $boldgreen
        ;;
      16)
        backup_persistent_config_file
        append_persistent_config "AWS_LC_SWITCH" "n" "$log_file"
        cecho "Nginx switched back to OpenSSL from AWS-LC" $boldgreen
        ;;
      17)
        backup_persistent_config_file
        append_persistent_config "NGXDYNAMIC_BROTLI" "n" "$log_file"
        append_persistent_config "NGINX_LIBBROTLI" "n" "$log_file"
        cecho "Nginx Brotli support disabled" $boldgreen
        ;;
      18)
        backup_persistent_config_file
        append_persistent_config "NGINX_ZSTD" "n" "$log_file"
        cecho "Nginx zstd support disabled" $boldgreen
        ;;
      19)
        backup_persistent_config_file
        append_persistent_config "NGX_LDMOLD" "n" "$log_file"
        cecho "Nginx mold linker compile support disabled" $boldgreen
        ;;
      20)
        backup_persistent_config_file
        append_persistent_config "NGINX_PCRE_TWO" "n" "$log_file"
        cecho "Nginx pcre2 support disabled" $boldgreen
        ;;
      21)
        backup_persistent_config_file
        append_persistent_config "NGINX_NJS" "n" "$log_file"
        append_persistent_config "NGXDYNAMIC_NJS" "n" "$log_file"
        cecho "Nginx njs module support disabled" $boldgreen
        ;;
      22)
        backup_persistent_config_file
        append_persistent_config "NGINX_VIDEO" "n" "$log_file"
        cecho "Nginx Video modules disabled (Slice, RTMP, FLV, MP4)" $boldgreen
        ;;
      23)
        backup_persistent_config_file
        append_persistent_config "PHP_LDMOLD" "n" "$log_file"
        cecho "PHP-FPM mold linker compile support disabled" $boldgreen
        ;;
      24)
        break
        ;;
      *)
        cecho "Invalid option" $boldred
        ;;
    esac
  done
}
