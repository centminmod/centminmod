#!/bin/bash

version_to_number() {
    echo "$@" | awk -F. '{ printf("%d%03d%03d%03d\n", $1,$2,$3,$4); }'
}

get_latest_aws_lc_version() {
    local latest_version
    latest_version=$(curl -s "https://api.github.com/repos/aws/aws-lc/tags?page=1&per_page=500" | jq -r '.[].name' | egrep -iv 'alpha|beta|rc|fips' | head -n1)
    if [[ -n "$latest_version" ]]; then
        echo "${latest_version#v}"
    else
        echo "1.30.1"  # Fallback version
    fi
}

get_latest_openssl_version() {
    local branch=$1
    local latest_version
    latest_version=$(curl -s "https://api.github.com/repos/openssl/openssl/tags?page=1&per_page=500" | jq -r '.[].name' | egrep -iv 'alpha|beta|rc|fips' | grep "^openssl-${branch}" | head -n1)
    if [[ -n "$latest_version" ]]; then
        echo "${latest_version#openssl-}"
    else
        case $branch in
            3.3) echo "3.3.1" ;;  # Fallback versions
            3.2) echo "3.2.2" ;;
            3.1) echo "3.1.5" ;;
            3.0) echo "3.0.14" ;;
            1.1.1) echo "1.1.1w" ;;
            *) echo "unknown" ;;
        esac
    fi
}

get_latest_libressl_version() {
    local branch=$1
    local latest_version
    latest_version=$(curl -s "https://api.github.com/repos/libressl/portable/tags?page=1&per_page=500" | jq -r '.[].name' | egrep -iv 'alpha|beta|rc|fips' | grep "^v${branch}" | head -n1)
    if [[ -n "$latest_version" ]]; then
        echo "${latest_version#v}"
    else
        case $branch in
            3.9) echo "3.9.2" ;;  # Fallback versions
            3.8) echo "3.8.4" ;;
            *) echo "unknown" ;;
        esac
    fi
}

check_nginx_crypto() {
    local nginx_v_output="$1"
    
    if [[ $nginx_v_output =~ "built with OpenSSL" ]]; then
        if [[ $nginx_v_output =~ "AWS-LC" ]]; then
            NGINX_CRYPTO_LIBRARY_USED="AWS-LC"
            NGINX_CRYPTO_LIBRARY_VERSION=$(echo "$nginx_v_output" | grep -oP 'AWS-LC \K[0-9.]+' | head -n1)
            local latest_version=$(get_latest_aws_lc_version)
            if [[ "$NGINX_CRYPTO_LIBRARY_VERSION" == "$latest_version" ]]; then
                NGINX_CRYPTO_LIBRARY_VERSION+=" (up to date)"
            else
                NGINX_CRYPTO_LIBRARY_VERSION+=" (update available $latest_version)"
            fi
        elif [[ $nginx_v_output =~ "BoringSSL" ]]; then
            NGINX_CRYPTO_LIBRARY_USED="BoringSSL"
            NGINX_CRYPTO_LIBRARY_VERSION="N/A"
        elif [[ $nginx_v_output =~ "quic" ]]; then
            NGINX_CRYPTO_LIBRARY_USED="quicTLS"
            NGINX_CRYPTO_LIBRARY_VERSION=$(echo "$nginx_v_output" | grep -oP 'OpenSSL \K[0-9.]+w?\+quic' | head -n1)
        elif [[ $nginx_v_output =~ "FIPS" ]]; then
            if [[ $nginx_v_output =~ "running with OpenSSL" ]]; then
                NGINX_CRYPTO_LIBRARY_USED="System-OpenSSL-FIPS"
            else
                NGINX_CRYPTO_LIBRARY_USED="OpenSSL-FIPS"
            fi
            NGINX_CRYPTO_LIBRARY_VERSION=$(echo "$nginx_v_output" | grep -oP 'OpenSSL \K[0-9.]+[a-z]?(?=.*FIPS)' | head -n1)
            NGINX_CRYPTO_LIBRARY_VERSION+=" (system FIPS version)"
        elif [[ $nginx_v_output =~ "built with OpenSSL" ]]; then
            NGINX_CRYPTO_LIBRARY_USED="OpenSSL"
            NGINX_CRYPTO_LIBRARY_VERSION=$(echo "$nginx_v_output" | grep -oP 'OpenSSL \K[0-9.]+' | head -n1)
            local current_major=$(echo $NGINX_CRYPTO_LIBRARY_VERSION | cut -d. -f1)
            local current_minor=$(echo $NGINX_CRYPTO_LIBRARY_VERSION | cut -d. -f2)
            local current_branch="$current_major.$current_minor"
            local latest_version=$(get_latest_openssl_version $current_branch)
            if [[ "$NGINX_CRYPTO_LIBRARY_VERSION" == "$latest_version" ]]; then
                NGINX_CRYPTO_LIBRARY_VERSION+=" (up to date)"
            elif [[ "$latest_version" != "unknown" ]]; then
                NGINX_CRYPTO_LIBRARY_VERSION+=" (update available $latest_version)"
            fi
        else
            NGINX_CRYPTO_LIBRARY_USED="Unknown"
            NGINX_CRYPTO_LIBRARY_VERSION="Unknown"
        fi
    elif [[ $nginx_v_output =~ "built with LibreSSL" ]]; then
        NGINX_CRYPTO_LIBRARY_USED="LibreSSL"
        NGINX_CRYPTO_LIBRARY_VERSION=$(echo "$nginx_v_output" | grep -oP 'LibreSSL \K[0-9.]+' | head -n1)
        local current_minor=$(echo $NGINX_CRYPTO_LIBRARY_VERSION | cut -d. -f2)
        local current_branch="3.$current_minor"
        local latest_version=$(get_latest_libressl_version $current_branch)
        if [[ "$NGINX_CRYPTO_LIBRARY_VERSION" == "$latest_version" ]]; then
            NGINX_CRYPTO_LIBRARY_VERSION+=" (up to date)"
        elif [[ "$latest_version" != "unknown" ]]; then
            NGINX_CRYPTO_LIBRARY_VERSION+=" (update available $latest_version)"
        fi
    else
        NGINX_CRYPTO_LIBRARY_USED="Unknown"
        NGINX_CRYPTO_LIBRARY_VERSION="Unknown"
    fi

    # Convert version to numeric format for internal use
    if [[ $NGINX_CRYPTO_LIBRARY_VERSION != "N/A" && $NGINX_CRYPTO_LIBRARY_VERSION != "Unknown" ]]; then
        NGINX_CRYPTO_LIBRARY_VERSION_NUMBER=$(version_to_number ${NGINX_CRYPTO_LIBRARY_VERSION%% *})
    else
        NGINX_CRYPTO_LIBRARY_VERSION_NUMBER=0
    fi
    if [[ "$NGINX_CRYPTO_LIBRARY_USED" != 'OpenSSL-FIPS' ]]; then
        echo "Nginx Crypto Library: $NGINX_CRYPTO_LIBRARY_USED $NGINX_CRYPTO_LIBRARY_VERSION"
    fi
}

check_version() {
    case $NGINX_CRYPTO_LIBRARY_USED in
        "OpenSSL")
            local current_major=$(echo $NGINX_CRYPTO_LIBRARY_VERSION | cut -d. -f1)
            local current_minor=$(echo $NGINX_CRYPTO_LIBRARY_VERSION | cut -d. -f2)
            local current_branch

            if (( current_major == 1 )); then
                current_branch="1.1.1"
                echo "Consider upgrading Nginx's crypto library to OpenSSL 3.x or alternative library"
                echo "https://community.centminmod.com/threads/25488/"
            elif (( current_major == 3 )); then
                current_branch="3.$current_minor"
            else
                echo "Unknown OpenSSL major version. Please check for updates manually."
                return
            fi

            local latest_version=$(get_latest_openssl_version $current_branch)
            if [[ "$latest_version" == "unknown" ]]; then
                echo "Unable to determine the latest version for OpenSSL $current_branch"
                echo "https://community.centminmod.com/threads/25488/"
            elif (( $(version_to_number $NGINX_CRYPTO_LIBRARY_VERSION) < $(version_to_number $latest_version) )); then
                echo "A newer version of Nginx's OpenSSL $current_branch is available: $latest_version"
                echo "https://community.centminmod.com/threads/25488/"
            fi
            ;;
        "OpenSSL-FIPS"|"System-OpenSSL-FIPS")
            # echo "This is a OpenSSL FIPS system version. Updates are managed by the YUM."
            return
            ;;
        "LibreSSL")
            local current_minor=$(echo $NGINX_CRYPTO_LIBRARY_VERSION | cut -d. -f2)
            local current_branch="3.$current_minor"

            local latest_version=$(get_latest_libressl_version $current_branch)
            if [[ "$latest_version" == "unknown" ]]; then
                echo "Unable to determine the latest version for LibreSSL $current_branch"
            elif (( $(version_to_number $NGINX_CRYPTO_LIBRARY_VERSION) < $(version_to_number $latest_version) )); then
                echo "A newer version of LibreSSL $current_branch is available: $latest_version"
                echo "https://community.centminmod.com/threads/25488/"
            fi

            # Check if there's a newer minor version available
            if (( current_minor == 8 )); then
                local latest_39=$(get_latest_libressl_version "3.9")
                echo "Consider upgrading to LibreSSL 3.9.x. Latest 3.9.x version: $latest_39"
                echo "https://community.centminmod.com/threads/25488/"
            fi
            ;;
        "AWS-LC")
            local latest_version=$(get_latest_aws_lc_version)
            if (( $(version_to_number $NGINX_CRYPTO_LIBRARY_VERSION) < $(version_to_number $latest_version) )); then
                echo "A newer version of AWS-LC is available: $latest_version"
                echo "https://community.centminmod.com/threads/25488/"
            fi
            ;;
    esac
}

# Get Nginx version info
nginx_version_info=$(nginx -V 2>&1)

# Check crypto library
check_nginx_crypto "$nginx_version_info"

# Check if a newer version is available
check_version

# Optional: Print full Nginx version info for reference
# echo "---"
# echo "Full Nginx version info:"
# echo "$nginx_version_info"