#!/bin/bash
#################################################################
# fix csf v14.18 adding DoT DNS over TLS port 853 which broke
# Centmin Mod initial install CSF port list configuration
# and didn't add pure-ftpd passive port range to whitelist
#################################################################
# Idempotent CSF TCP_IN/TCP6_IN normalizer. Ensures the non-FTP
# misc ports are always whitelisted, and adds/removes the
# pure-ftpd passive port range (30001:50011), the FTP control
# port 21 and the FTP PORTFLOOD rule according to
# PUREFTPD_DISABLED. Safe to run repeatedly: it only backs up
# csf.conf and reloads CSF when a line actually changes, so it
# can run on every centmin.sh / cmupdate pass without churn.
#
# This is a standalone script invoked by inc/cpcheck.inc before
# centmin.sh has sourced the persistent config, so it sources
# /etc/centminmod/custom_config.inc itself to learn the real
# PUREFTPD_DISABLED value (defaulting to 'n' when unset).
#################################################################
DT=$(date +"%d%m%y-%H%M%S")
CENTMINLOGDIR='/root/centminlogs'

PUREFTPD_DISABLED='n'
if [ -f /etc/centminmod/custom_config.inc ]; then
  source /etc/centminmod/custom_config.inc
fi

# non-FTP misc ports always whitelisted (memcached/varnish/git/openvpn etc.)
# NOTE: 2049/111 (NFS) are deliberately NOT managed here - rpcnfsports()
# in inc/csftweaks.inc handles them so the two routines don't fight.
MISC_TCP_PORTS="1110 1186 1194 81 9418"
FTP_PASSIVE_RANGE="30001:50011"

# pure-ftpd actively RUNNING? (guards REMOVAL of the passive range / port 21).
# Must be a running check, not an installed check: the default install-then-disable
# flow leaves pure-ftpd installed on every disabled host, so an installed-based
# guard would never remove the stale range from the hosts that actually have it.
pureftpd_running() {
  if command -v systemctl >/dev/null 2>&1; then
    systemctl is-active --quiet pure-ftpd && return 0
  fi
  ps aufx | grep -v grep | grep -q 'pure-ftpd' && return 0
  return 1
}

# is a NON-pure-ftpd FTP daemon (vsftpd/proftpd) installed? (an installed check is
# right here: don't close stock control port 21 if another FTP daemon could use it)
other_ftpd_present() {
  rpm -q vsftpd >/dev/null 2>&1 && return 0
  rpm -q proftpd >/dev/null 2>&1 && return 0
  command -v vsftpd >/dev/null 2>&1 && return 0
  command -v proftpd >/dev/null 2>&1 && return 0
  return 1
}

# recompute a comma-separated port list:
#   $1 = current value (comma separated)
#   $2 = ensure ports (space separated) - appended to END if missing
#   $3 = remove ports (space separated)
# drops empty tokens, removes the remove-set, dedupes preserving first-seen
# order, then appends any missing ensure ports. Never reorders existing
# tokens, so a clean list is returned byte-identical (idempotent).
recompute_list() {
  awk -v cur="$1" -v ensure="$2" -v remove="$3" '
  BEGIN {
    nr = split(remove, ra, " ")
    for (i = 1; i <= nr; i++) if (ra[i] != "") rm[ra[i]] = 1
    n = split(cur, t, ",")
    out = ""; cnt = 0
    for (i = 1; i <= n; i++) {
      tok = t[i]
      if (tok == "") continue
      if (tok in rm) continue
      if (tok in seen) continue
      seen[tok] = 1
      out = (cnt++ ? out "," tok : tok)
    }
    ne = split(ensure, ea, " ")
    for (i = 1; i <= ne; i++) {
      ep = ea[i]
      if (ep == "") continue
      if (ep in rm) continue
      if (ep in seen) continue
      seen[ep] = 1
      out = (cnt++ ? out "," ep : ep)
    }
    print out
  }'
}

fix_csf_ftp_ports() {
  [ -f /etc/csf/csf.conf ] || return 0

  local ensure="$MISC_TCP_PORTS" remove=""
  if [[ "$PUREFTPD_DISABLED" = [yY] ]]; then
    # do not add the passive range. Remove it (and possibly port 21) only when
    # pure-ftpd is NOT running - if an admin manually re-enabled and started it
    # but left the flag at y, leave the ports alone so active FTP keeps working.
    if ! pureftpd_running; then
      remove="$remove $FTP_PASSIVE_RANGE"
      # also drop stock control port 21, but only if no other FTP daemon
      # (vsftpd/proftpd) is installed that might legitimately use it
      if ! other_ftpd_present; then
        remove="$remove 21"
      fi
    fi
  else
    ensure="$ensure $FTP_PASSIVE_RANGE"
  fi

  local changed=0 var line cur newval_tcpin="" newval_tcp6in=""
  for var in TCP_IN TCP6_IN; do
    line=$(grep -E "^${var} = \"" /etc/csf/csf.conf)
    [ -z "$line" ] && continue
    cur=$(printf '%s' "$line" | sed -E 's/^[^"]*"//; s/".*$//')
    # skip an empty list (e.g. TCP6_IN="" on IPv6-disabled hosts)
    [ -z "$cur" ] && continue
    local new
    new=$(recompute_list "$cur" "$ensure" "$remove")
    if [ "$new" != "$cur" ]; then
      [ "$var" = 'TCP_IN' ] && newval_tcpin="$new"
      [ "$var" = 'TCP6_IN' ] && newval_tcp6in="$new"
      changed=1
    fi
  done

  # FTP PORTFLOOD rule removal when disabled (covers existing hosts toggled
  # to PUREFTPD_DISABLED=y after install). Match count/interval generically
  # because CSFPORTFLOOD_OVERRIDE can change them from the 20;300 default.
  local portflood_change=0
  if [[ "$PUREFTPD_DISABLED" = [yY] ]]; then
    if grep -E '^PORTFLOOD = ' /etc/csf/csf.conf | grep -qE '21;tcp;[0-9]+;[0-9]+'; then
      portflood_change=1
      changed=1
    fi
  fi

  [ "$changed" -eq 0 ] && return 0

  # backup existing CSF Firewall profile config at /var/lib/csf/backup/
  csf --profile backup fix-ftp >/dev/null 2>&1
  [ -n "$newval_tcpin" ]  && sed -i "s|^TCP_IN = \".*\"|TCP_IN = \"${newval_tcpin}\"|" /etc/csf/csf.conf
  [ -n "$newval_tcp6in" ] && sed -i "s|^TCP6_IN = \".*\"|TCP6_IN = \"${newval_tcp6in}\"|" /etc/csf/csf.conf
  if [ "$portflood_change" -eq 1 ]; then
    sed -i -E '/^PORTFLOOD = /{
      s/21;tcp;[0-9]+;[0-9]+//
      s/,,/,/g
      s/"(,)/"/
      s/(,)"/"/
    }' /etc/csf/csf.conf
  fi
  # show resulting CSF port config and reload CSF firewall + lfd
  grep -E '^TCP_|^TCP6_|^UDP_|^UDP6_|^PORTFLOOD' /etc/csf/csf.conf
  csf -ra
}

{
  fix_csf_ftp_ports
} 2>&1 | tee "${CENTMINLOGDIR}/fix-csf-ftp-ports-${DT}.log"
