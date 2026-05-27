#!/bin/bash
# cmsec-prewarm.sh — pre-warm the cmsec CVE detection cache so SSH logins
# don't pay the cold-cache cost when config/motd/dmotd.sh runs.
#
# Designed to be run from root's crontab. The cron entry is auto-installed
# by cmsec_prewarm_cronsetup() in inc/cpcheck.inc on every centmin.sh run
# (covers both fresh installs and existing-user cmupdate paths). Admins
# can also run this script manually right after a kernel reboot, a
# kcarectl --update livepatch apply, or an in-place OS conversion — those
# events invalidate the cache state-key, and a manual run closes the
# resulting gap before the next scheduled tick.
#
# Why --no-cache --dmotd --json:
#   --no-cache : forces a fresh detection run; a plain --json would
#                short-circuit on a still-valid cache and not refresh.
#   --dmotd    : enables the stale-fallback + setsid background-refresh
#                code path at tools/cmm-security/cmsec.sh:206. While the
#                stale-fallback isn't useful during an explicit --no-cache
#                refresh, keeping the flag set ensures DMOTD_CVECHECK_SUPPRESS
#                semantics match what dmotd.sh consumes.
#   --json     : silences the per-CVE narrative; we only want the cache
#                populated, not the banner.

set -u

CMSCRIPT_GITDIR='/usr/local/src/centminmod'
CMSEC_BIN="${CMSCRIPT_GITDIR}/tools/cmm-security/cmsec.sh"
PREWARM_LOCK='/var/cache/centminmod/cmsec/.prewarm.lock'

# Bail quietly if cmsec.sh is missing (e.g. partial install). Exit 0 so
# the cron entry doesn't generate root mail on every tick.
[ -x "$CMSEC_BIN" ] || exit 0

mkdir -p "$(dirname "$PREWARM_LOCK")" 2>/dev/null
exec 9>"$PREWARM_LOCK" 2>/dev/null || exit 0

if command -v flock >/dev/null 2>&1; then
  # Non-blocking: if a previous prewarm is still running, just exit. The
  # next cron tick will retry; meanwhile the running invocation will
  # populate the cache.
  flock -n 9 || exit 0
fi

exec "$CMSEC_BIN" --no-cache --dmotd --json >/dev/null 2>&1
