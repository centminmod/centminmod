#!/bin/bash
export PATH="/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin:/root/bin"
# disable systemd pager so it doesn't pipe systemctl output to less
export SYSTEMD_PAGER=''
######################################################
# Anubis proof-of-work bot challenge for nginx (auth_request subrequest mode)
# https://github.com/TecharoHQ/anubis
# written by George Liu (eva2000) centminmod.com
######################################################
# Optional addon, never run automatically. Installs the upstream Anubis x86_64
# RPM and runs one instance, anubis@centminmod, listening on 127.0.0.1 only:
#   /etc/anubis/centminmod.env               upstream default.env with BIND,
#                                            METRICS_BIND, TARGET=" " (subrequest
#                                            mode), POLICY_FNAME and a persistent
#                                            ED25519_PRIVATE_KEY_HEX (mode 0600)
#   /etc/anubis/centminmod.botPolicies.yaml  upstream botPolicies.yaml with
#                                            status_codes DENY: 403 (nginx treats
#                                            the upstream DENY: 200 as a pass)
#   /usr/local/nginx/conf/anubis.conf        helper locations, once per server {}
#   /usr/local/nginx/conf/anubis_check.conf  auth_request lines, server {} or
#                                            chosen locations
# Existing files are never overwritten, so local edits survive a reinstall.
# nginx needs the auth_request module: NGINX_AUTHREQ='y' in
# /etc/centminmod/custom_config.inc, then a centmin.sh menu 4 nginx rebuild.
#
# usage:
#   addons/anubis.sh install    install or upgrade the RPM, write config once, (re)start
#   addons/anubis.sh status     package, service, check endpoint, nginx auth_request, vhost includes
#   addons/anubis.sh uninstall  stop and remove the RPM, refused while a vhost includes anubis*.conf
######################################################
DT=$(date +"%d%m%y-%H%M%S")
CENTMINLOGDIR='/root/centminlogs'
DIR_TMP='/svr-setup'
CONFIGSCANBASE='/etc/centminmod'

ANUBIS_VER_DEFAULT='1.28.2'
# sha256 of the default version's x86_64 RPM, a different ANUBIS_VER skips the check
ANUBIS_RPM_SHA256_DEFAULT='62fa6e9d66480b70f24e14655a17589f2a83e81c9015fc698c4d9492777cb52e'
ANUBIS_VER="$ANUBIS_VER_DEFAULT"
ANUBIS_INSTANCE='centminmod'
ANUBIS_PORT='8923'
# upstream default 9090 is also Cockpit's port
ANUBIS_METRICS_PORT='8924'
######################################################
# Setup Colours
boldgreen='\E[1;32;40m'
boldyellow='\E[1;33;40m'
boldred='\E[1;31;40m'

Reset="tput sgr0"      #  Reset text attributes to normal
                       #+ without clearing screen.

cecho ()                     # Coloured-echo.
                             # Argument $1 = message
                             # Argument $2 = color
{
message=$1
color=$2
echo -e "$color$message" ; $Reset 2>/dev/null
return
}
######################################################
# ANUBIS_VER, ANUBIS_PORT and ANUBIS_METRICS_PORT can be set persistently
if [ -f "${CONFIGSCANBASE}/custom_config.inc" ]; then
  source "${CONFIGSCANBASE}/custom_config.inc"
fi

ANUBIS_RPM="anubis-${ANUBIS_VER}-1.x86_64.rpm"
ANUBIS_RPM_URL="https://github.com/TecharoHQ/anubis/releases/download/v${ANUBIS_VER}/${ANUBIS_RPM}"
if [[ "$ANUBIS_VER" = "$ANUBIS_VER_DEFAULT" ]]; then
  ANUBIS_RPM_SHA256="$ANUBIS_RPM_SHA256_DEFAULT"
else
  ANUBIS_RPM_SHA256=''
fi
ANUBIS_ENV="/etc/anubis/${ANUBIS_INSTANCE}.env"
ANUBIS_POLICY="/etc/anubis/${ANUBIS_INSTANCE}.botPolicies.yaml"
ANUBIS_SERVICE="anubis@${ANUBIS_INSTANCE}"
SCRIPT_DIR=$(readlink -f "$(dirname "$0")")
CM_INSTALLDIR=$(dirname "$SCRIPT_DIR")

if [ ! -d "$CENTMINLOGDIR" ]; then
  mkdir -p "$CENTMINLOGDIR"
fi

anubis_precheck() {
  if [[ "$(id -u)" -ne '0' ]]; then
    cecho "run as root" $boldred
    return 1
  fi
  if [[ "$(uname -m)" != 'x86_64' ]]; then
    cecho "Anubis addon supports x86_64 only" $boldred
    return 1
  fi
  if [[ ! -d /etc/systemd/system ]] || ! command -v dnf >/dev/null 2>&1; then
    cecho "Anubis addon needs EL8, EL9 or EL10 (systemd and dnf)" $boldred
    return 1
  fi
  return 0
}

anubis_rpm_ok() {
  # non-empty RPM file and, for the default version, the expected sha256
  [[ -s "$1" ]] || return 1
  rpm -qp "$1" >/dev/null 2>&1 || return 1
  if [ -n "$ANUBIS_RPM_SHA256" ]; then
    [[ "$(sha256sum "$1" | awk '{print $1}')" = "$ANUBIS_RPM_SHA256" ]] || return 1
  fi
  return 0
}

anubis_env_set() {
  # anubis_env_set <key> <value>: replace an existing KEY= line or append one
  if grep -q "^${1}=" "$ANUBIS_ENV"; then
    sed -i "s|^${1}=.*|${1}=${2}|" "$ANUBIS_ENV"
  else
    echo "${1}=${2}" >> "$ANUBIS_ENV"
  fi
}

anubis_nginx_check() {
  # 0 when the running nginx binary has the auth_request module
  if nginx -V 2>&1 | grep -q -- '--with-http_auth_request_module'; then
    cecho "nginx has the auth_request module" $boldgreen
    return 0
  fi
  cecho "nginx was built without the auth_request module, which Anubis subrequest mode needs" $boldyellow
  cecho "rebuild nginx with it:" $boldyellow
  if grep -qs "^NGINX_AUTHREQ=" "${CONFIGSCANBASE}/custom_config.inc"; then
    echo "  1. set NGINX_AUTHREQ='y' in ${CONFIGSCANBASE}/custom_config.inc (now: $(grep "^NGINX_AUTHREQ=" "${CONFIGSCANBASE}/custom_config.inc" | tail -1))"
  else
    echo "  1. echo \"NGINX_AUTHREQ='y'\" >> ${CONFIGSCANBASE}/custom_config.inc"
  fi
  echo "  2. centmin.sh menu option 4 (Nginx Upgrade / Downgrade), the same nginx version rebuilds it"
  echo "  3. nginx -V 2>&1 | grep -o with-http_auth_request_module"
  return 1
}

anubis_vhost_includes() {
  # vhost and nginx config files with an active include of anubis*.conf, plus
  # files the running config loads with the Anubis auth_request check: nginx -T
  # also catches wildcard includes and renamed copies of the templates
  {
    grep -rlE --include='*.conf' '^[[:space:]]*include[[:space:]]+[^;#]*anubis(_check)?\.conf' /usr/local/nginx/conf 2>/dev/null
    if command -v nginx >/dev/null 2>&1; then
      nginx -T 2>/dev/null | awk '/^# configuration file .*:$/ { f = substr($0, 22); sub(/:$/, "", f) }
        /^[[:space:]]*auth_request[[:space:]]+\/\.within\.website\/x\/cmd\/anubis\/api\/check;/ { print f }'
    fi
  } | sort -u
}

anubis_install() {
  anubis_precheck || return 1
  echo
  cecho "Install Anubis ${ANUBIS_VER} (${ANUBIS_RPM})" $boldyellow
  mkdir -p "${DIR_TMP}/anubis"
  local rpmfile="${DIR_TMP}/anubis/${ANUBIS_RPM}"
  if anubis_rpm_ok "$rpmfile"; then
    cecho "${ANUBIS_RPM} found, skipping download..." $boldgreen
  else
    rm -f "$rpmfile"
    echo "wget --progress=bar ${ANUBIS_RPM_URL} -O ${rpmfile} --tries=3"
    wget --progress=bar "$ANUBIS_RPM_URL" -O "$rpmfile" --tries=3
    if ! anubis_rpm_ok "$rpmfile"; then
      cecho "Error: ${ANUBIS_RPM} download failed, invalid RPM or sha256 mismatch" $boldred
      rm -f "$rpmfile"
      return 1
    fi
  fi
  if [ -z "$ANUBIS_RPM_SHA256" ]; then
    cecho "ANUBIS_VER='${ANUBIS_VER}' is not the default ${ANUBIS_VER_DEFAULT}, RPM sha256 not checked" $boldyellow
  fi
  # installs, or upgrades an older installed version
  dnf -y install "$rpmfile" || return 1
  if [[ ! -x /usr/bin/anubis || ! -f /etc/anubis/default.env || ! -f /usr/share/doc/anubis/botPolicies.yaml ]]; then
    cecho "Error: anubis RPM files missing (/usr/bin/anubis, /etc/anubis/default.env or /usr/share/doc/anubis/botPolicies.yaml)" $boldred
    return 1
  fi

  if [ ! -f "$ANUBIS_ENV" ]; then
    \cp -f /etc/anubis/default.env "$ANUBIS_ENV"
    chmod 0600 "$ANUBIS_ENV"
    # upstream default.env has no trailing newline, appended keys need one
    sed -i -e '$a\' "$ANUBIS_ENV"
    # loopback only, upstream binds every interface
    anubis_env_set BIND "127.0.0.1:${ANUBIS_PORT}"
    anubis_env_set METRICS_BIND "127.0.0.1:${ANUBIS_METRICS_PORT}"
    # a single space: subrequest mode, nginx serves the site itself
    anubis_env_set TARGET '" "'
    anubis_env_set POLICY_FNAME "$ANUBIS_POLICY"
    # persistent signing key, so passed challenges survive restarts and upgrades
    anubis_env_set ED25519_PRIVATE_KEY_HEX "$(openssl rand -hex 32)"
    cecho "created ${ANUBIS_ENV}" $boldgreen
  else
    cecho "${ANUBIS_ENV} exists, left unchanged" $boldgreen
  fi

  if [ ! -f "$ANUBIS_POLICY" ]; then
    \cp -f /usr/share/doc/anubis/botPolicies.yaml "$ANUBIS_POLICY"
    chmod 0644 "$ANUBIS_POLICY"
    # nginx auth_request treats any 2xx as a pass, so DENY must not be 200
    sed -i 's|^\([[:space:]]*\)DENY: 200[[:space:]]*$|\1DENY: 403|' "$ANUBIS_POLICY"
    if ! grep -qE '^[[:space:]]*DENY: 403' "$ANUBIS_POLICY"; then
      if ! grep -q '^status_codes:' "$ANUBIS_POLICY"; then
        printf '\nstatus_codes:\n  CHALLENGE: 200\n  DENY: 403\n' >> "$ANUBIS_POLICY"
      else
        cecho "Error: could not set status_codes DENY: 403 in ${ANUBIS_POLICY}, edit it by hand" $boldred
        return 1
      fi
    fi
    cecho "created ${ANUBIS_POLICY} (status_codes DENY: 403)" $boldgreen
  else
    cecho "${ANUBIS_POLICY} exists, left unchanged" $boldgreen
  fi

  local ngxconf
  for ngxconf in anubis.conf anubis_check.conf; do
    # centmin.sh initial installs copy config/nginx, so an anubis.conf identical
    # to the shipped one still gets a non-default ANUBIS_PORT
    if [[ -d /usr/local/nginx/conf ]] && { [[ ! -f "/usr/local/nginx/conf/${ngxconf}" ]] || { [[ "$ngxconf" = 'anubis.conf' && "$ANUBIS_PORT" != '8923' ]] && cmp -s "${CM_INSTALLDIR}/config/nginx/${ngxconf}" "/usr/local/nginx/conf/${ngxconf}"; }; }; then
      if [ -f "${CM_INSTALLDIR}/config/nginx/${ngxconf}" ]; then
        \cp -f "${CM_INSTALLDIR}/config/nginx/${ngxconf}" "/usr/local/nginx/conf/${ngxconf}"
        if [[ "$ngxconf" = 'anubis.conf' && "$ANUBIS_PORT" != '8923' ]]; then
          sed -i "s|127.0.0.1:8923|127.0.0.1:${ANUBIS_PORT}|g" "/usr/local/nginx/conf/${ngxconf}"
        fi
        cecho "created /usr/local/nginx/conf/${ngxconf}" $boldgreen
      else
        cecho "${CM_INSTALLDIR}/config/nginx/${ngxconf} not found, run cmupdate" $boldyellow
      fi
    fi
  done

  systemctl daemon-reload
  systemctl enable "$ANUBIS_SERVICE"
  # restart picks up an upgraded binary
  systemctl restart "$ANUBIS_SERVICE"
  sleep 2
  if ! systemctl is-active --quiet "$ANUBIS_SERVICE"; then
    cecho "Error: ${ANUBIS_SERVICE} did not start" $boldred
    journalctl -u "$ANUBIS_SERVICE" --no-pager -n 20
    return 1
  fi
  cecho "${ANUBIS_SERVICE} running on 127.0.0.1:${ANUBIS_PORT}" $boldgreen
  echo
  anubis_nginx_check
  echo
  cecho "Protect a vhost: in its server {} block add" $boldyellow
  echo "  include /usr/local/nginx/conf/anubis.conf;"
  echo "  include /usr/local/nginx/conf/anubis_check.conf;"
  echo "then: nginx -t && nprestart"
  echo "HTTPS vhosts only unless COOKIE_SECURE=false is set in ${ANUBIS_ENV}, see /usr/local/nginx/conf/anubis.conf"
  return 0
}

anubis_status() {
  echo
  if rpm -q anubis >/dev/null 2>&1; then
    cecho "package: $(rpm -q anubis)" $boldgreen
  else
    cecho "package: anubis not installed" $boldyellow
  fi
  if systemctl is-active --quiet "$ANUBIS_SERVICE"; then
    cecho "service: ${ANUBIS_SERVICE} active" $boldgreen
  else
    cecho "service: ${ANUBIS_SERVICE} not active" $boldyellow
  fi
  systemctl status "$ANUBIS_SERVICE" --no-pager -l 2>/dev/null | head -n 12
  echo
  if [ -f "$ANUBIS_ENV" ]; then
    echo "${ANUBIS_ENV}:"
    grep -E '^(BIND|METRICS_BIND|TARGET|POLICY_FNAME|DIFFICULTY|COOKIE_SECURE)=' "$ANUBIS_ENV" | sed -e 's|^|  |'
  fi
  if [ -f "$ANUBIS_POLICY" ]; then
    echo "${ANUBIS_POLICY} status_codes:"
    grep -A2 '^status_codes:' "$ANUBIS_POLICY" | sed -e 's|^|  |'
  fi
  local check_port check_code
  check_port=$(awk -F ':' '/^BIND=/ {print $NF}' "$ANUBIS_ENV" 2>/dev/null)
  check_code=$(curl -s -o /dev/null -w '%{http_code}' -H 'X-Real-IP: 127.0.0.1' "http://127.0.0.1:${check_port:-$ANUBIS_PORT}/.within.website/x/cmd/anubis/api/check")
  echo "check endpoint http://127.0.0.1:${check_port:-$ANUBIS_PORT}/.within.website/x/cmd/anubis/api/check (curl, no cookie): HTTP ${check_code}"
  echo
  anubis_nginx_check
  echo
  local vhosts
  vhosts=$(anubis_vhost_includes)
  if [ -n "$vhosts" ]; then
    echo "nginx config files including anubis*.conf:"
    echo "$vhosts" | sed -e 's|^|  |'
  else
    echo "no nginx config file includes anubis*.conf"
  fi
  return 0
}

anubis_uninstall() {
  anubis_precheck || return 1
  local vhosts
  vhosts=$(anubis_vhost_includes)
  if [ -n "$vhosts" ]; then
    cecho "These nginx config files still include anubis*.conf, and every protected request" $boldred
    cecho "would fail with 500 once Anubis stops. Remove those include lines first:" $boldred
    echo "$vhosts" | sed -e 's|^|  |'
    echo "then: nginx -t && nprestart, and run $0 uninstall again"
    return 1
  fi
  systemctl disable --now "$ANUBIS_SERVICE" 2>/dev/null
  if rpm -q anubis >/dev/null 2>&1; then
    dnf -y remove anubis || return 1
  fi
  systemctl daemon-reload
  cecho "Anubis removed. Kept for a later reinstall, delete them by hand if not needed:" $boldgreen
  echo "  ${ANUBIS_ENV} (holds the signing key)"
  echo "  ${ANUBIS_POLICY}"
  echo "  /usr/local/nginx/conf/anubis.conf /usr/local/nginx/conf/anubis_check.conf"
  return 0
}

######################################################
case "$1" in
  install )
    anubis_install 2>&1 | tee "${CENTMINLOGDIR}/centminmod_anubis_install_${DT}.log"
    exit "${PIPESTATUS[0]}"
    ;;
  status )
    anubis_status
    ;;
  uninstall )
    anubis_uninstall 2>&1 | tee "${CENTMINLOGDIR}/centminmod_anubis_uninstall_${DT}.log"
    exit "${PIPESTATUS[0]}"
    ;;
  * )
    echo
    echo "Usage:"
    echo
    echo "$0 {install|status|uninstall}"
    echo
    ;;
esac

exit
