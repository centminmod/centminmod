#!/bin/bash
export PATH="/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin:/root/bin"
#################################################################
# acme.sh --pre-hook script
#################################################################
DT=$(date +"%d%m%y-%H%M%S")
SCRIPTDIR="$(cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd)"
BASEDIR=$(dirname $SCRIPTDIR)

# set locale temporarily to english
# due to some non-english locale issues
export LC_ALL=en_US.UTF-8
export LANG=en_US.UTF-8
export LANGUAGE=en_US.UTF-8
export LC_CTYPE=en_US.UTF-8
# disable systemd pager so it doesn't pipe systemctl output to less
export SYSTEMD_PAGER=''

shopt -s expand_aliases
for g in "" e f; do
    alias ${g}grep="LC_ALL=C ${g}grep"  # speed-up grep, egrep, fgrep
done

webroot_checks() {
  CHECK_MODE=$1
  WEBROOT_DOMAIN=$2
  # Acme.sh configuration file
  ACME_CONF_FILE="/root/.acme.sh/${WEBROOT_DOMAIN}/${WEBROOT_DOMAIN}.conf"
  ACME_CONF_FILE_ECC="/root/.acme.sh/${WEBROOT_DOMAIN}_ecc/${WEBROOT_DOMAIN}.conf"
  
  # Nginx configuration file
  NGINX_CONF_FILE="/usr/local/nginx/conf/conf.d/${WEBROOT_DOMAIN}.ssl.conf"

  # Check if the acme.sh configuration file exists
  if [[ -f "$ACME_CONF_FILE" && -f "$NGINX_CONF_FILE" ]]; then
    # Get the root path from the nginx configuration
    NGINX_ROOT_PATH=$(grep -v '^#' $NGINX_CONF_FILE | grep -m1 'root' | awk '{print $2}' | tr -d ';')
    echo "Nginx root path: $NGINX_ROOT_PATH"
  
    # Get the Le_Webroot value from the acme.sh configuration
    LE_WEBROOT=$(grep 'Le_Webroot' $ACME_CONF_FILE | cut -d'=' -f2 | tr -d "'")
    echo "Le_Webroot: $LE_WEBROOT"
    if [[ -f "$ACME_CONF_FILE_ECC" ]]; then
      LE_WEBROOT_ECC=$(grep 'Le_Webroot' $ACME_CONF_FILE_ECC | cut -d'=' -f2 | tr -d "'")
      echo "ECC Le_Webroot: $LE_WEBROOT_ECC"
    fi
  
    # If Le_Webroot contains comma-separated values, get the second one
    if [[ "$LE_WEBROOT" == *","* ]]; then
      LE_WEBROOT=$(echo "$LE_WEBROOT" | cut -d',' -f2 | tr -d "'")
    fi
    if [[ -f "$ACME_CONF_FILE_ECC" ]]; then
      if [[ "$LE_WEBROOT_ECC" == *","* ]]; then
        LE_WEBROOT_ECC=$(echo "$LE_WEBROOT_ECC" | cut -d',' -f2 | tr -d "'")
      fi
    fi
  
    # Check if the paths match
    if [[ "$LE_WEBROOT" != "$NGINX_ROOT_PATH" ]] && [[ "$LE_WEBROOT" != 'dns_cf' ]]; then
      echo "Error: The root paths in the acme.sh and nginx configurations do not match. Updating the acme.sh configuration..."
      # Make a backup of the original acme.sh configuration
      if [[ "$CHECK_MODE" = 'liverun' ]]; then
        cp "$ACME_CONF_FILE" "${ACME_CONF_FILE}-${DT}.bak"
        if [[ -f "$ACME_CONF_FILE_ECC" ]]; then
          cp "$ACME_CONF_FILE_ECC" "${ACME_CONF_FILE_ECC}-${DT}.bak"
        fi
      else
        echo "cp $ACME_CONF_FILE ${ACME_CONF_FILE}-${DT}.bak"
        if [[ -f "$ACME_CONF_FILE_ECC" ]]; then
          echo "cp $ACME_CONF_FILE_ECC ${ACME_CONF_FILE_ECC}-${DT}.bak"
        fi
      fi
      # Update the Le_Webroot value in the acme.sh configuration
      if [[ "$CHECK_MODE" = 'liverun' ]]; then
        sed -i "s|$LE_WEBROOT|$NGINX_ROOT_PATH|g" "$ACME_CONF_FILE"
        if [[ -f "$ACME_CONF_FILE_ECC" ]]; then
          sed -i "s|$LE_WEBROOT_ECC|$NGINX_ROOT_PATH|g" "$ACME_CONF_FILE_ECC"
        fi
      else
        echo "sed -i \"s|$LE_WEBROOT|$NGINX_ROOT_PATH|g\" \"$ACME_CONF_FILE\""
        if [[ -f "$ACME_CONF_FILE_ECC" ]]; then
          echo "sed -i \"s|$LE_WEBROOT_ECC|$NGINX_ROOT_PATH|g\" \"$ACME_CONF_FILE_ECC\""
        fi
      fi
    else
      echo "The root paths match. Proceeding with the acme.sh operation."
    fi
    return 0
  else
    echo "The acme.sh configuration file ${ACME_CONF_FILE} does not exist or"
    echo "The Nginx HTTPS vhost configuration file ${NGINX_CONF_FILE} does not exist"
    return 1
  fi
}

case "$1" in
    all-check )
        webroot_checks liverun "$2"
        ;;
    all-check-dryrun )
        webroot_checks dryrun "$2"
        ;;
    webroot-check )
        webroot_checks liverun "$2"
        ;;
    webroot-check-dryrun )
        webroot_checks dryrun "$2"
        ;;
    * )
    echo "$0 all-check domain.com"
    echo "$0 all-check-dryrun domain.com"
    echo "$0 webroot-check domain.com"
    echo "$0 webroot-check-dryrun domain.com"
        ;;
esac