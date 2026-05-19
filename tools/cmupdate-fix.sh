#!/bin/bash
######################################################
# cmupdate-fix.sh
# Centmin Mod recovery script for broken cmupdate
# (triggered when the upstream branch was force-pushed
# / rebased on GitHub and `git pull` fails with
# "fatal: Not possible to fast-forward, aborting.")
#
# Usage:
#   curl -fsSLo /tmp/cmupdate-fix.sh \
#     https://raw.githubusercontent.com/centminmod/centminmod/141.00beta01/tools/cmupdate-fix.sh \
#     && bash /tmp/cmupdate-fix.sh
#
# Flags:
#   --yes / -y       Skip the interactive ENTER prompt (for scripted use).
#
# Standalone by design: does NOT source inc/*.inc, so it works
# on a broken local tree.
######################################################
export PATH="/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin:/root/bin"
export LC_ALL=en_US.UTF-8
export LANG=en_US.UTF-8

CM_INSTALLDIR='/usr/local/src/centminmod'
FORUM_URL='https://community.centminmod.com/forums/install-upgrades-or-pre-install-questions.8/'

YES=0
for arg in "$@"; do
  case "$arg" in
    --yes|-y) YES=1 ;;
  esac
done

bar() { echo "================================================================"; }
result_fail() {
  local step="$1" reason="$2"
  bar
  echo " RESULT: FAILED — $step"
  echo "   $reason"
  echo "   Your local code at $CM_INSTALLDIR was NOT modified."
  echo "   Get help: $FORUM_URL"
  bar
  exit 1
}

translate_git_error() {
  # Plain-English mapping for the three common git failures users hit.
  case "$1" in
    *'Not possible to fast-forward'*|*'Diverging branches'*)
      echo "GitHub rewrote this branch's history (force-push / rebase)." ;;
    *'refusing to merge unrelated histories'*)
      echo "Local clone and upstream have no common ancestor (corrupted clone)." ;;
    *'local changes would be overwritten'*|*'commit your changes or stash them'*)
      echo "Local uncommitted changes block the update." ;;
    *'Could not resolve host'*|*'Failed to connect'*)
      echo "Network problem talking to GitHub. Check connectivity and retry." ;;
    *)
      echo "$1" ;;
  esac
}

bar
echo " Centmin Mod cmupdate recovery script"
bar

if [ ! -d "$CM_INSTALLDIR/.git" ]; then
  result_fail "no git checkout" "$CM_INSTALLDIR/.git not found — Centmin Mod not installed via git, or the directory was wiped."
fi

cd "$CM_INSTALLDIR" || result_fail "cd" "Could not enter $CM_INSTALLDIR"

# Step A: detect local branch.
branch=$(git symbolic-ref --short HEAD 2>/dev/null)
if [ -z "$branch" ] || [ "$branch" = "HEAD" ]; then
  # Detached HEAD or missing — fall back to parsing centmin.sh.
  branch=$(awk -F"'" '/^branchname=/ {print $2; exit}' "$CM_INSTALLDIR/centmin.sh" 2>/dev/null)
fi
if [ -z "$branch" ]; then
  result_fail "branch detection" "Could not determine which Centmin Mod branch this clone is on. Known active branches: 141.00beta01, 140.00beta01, 132.00stable."
fi

local_sha=$(git rev-parse --short HEAD 2>/dev/null || echo "unknown")

echo " Your Centmin Mod local code at $CM_INSTALLDIR is out"
echo " of sync because the upstream branch was rebuilt on GitHub."
echo " This script will reset your local copy to match."
echo
echo " Detected local branch: $branch"
echo " Local commit:  $local_sha (your current code)"
echo
echo " Local changes (if any) will be saved to a git stash and a"
echo " recovery tag, so nothing is lost permanently."
echo
if [ "$YES" -ne 1 ]; then
  echo " Press ENTER to continue, or Ctrl-C to abort."
  bar
  read -r _
else
  echo " --yes given: continuing without prompt."
  bar
fi
echo

# Step 1/4: fetch
echo -n " [1/4] Fetching latest $branch from GitHub...        "
fetch_err=$(git fetch --prune origin "$branch" 2>&1 >/dev/null)
if [ $? -ne 0 ]; then
  echo "FAIL"
  result_fail "fetch" "$(translate_git_error "$fetch_err")"
fi
echo "OK"

if ! git rev-parse --verify --quiet "origin/$branch" >/dev/null; then
  result_fail "fetch" "Branch '$branch' not found on the remote. Has it been renamed or deleted?"
fi
remote_sha=$(git rev-parse --short "origin/$branch")
echo "       Remote commit: $remote_sha (the new upstream)"

# Step 2/4: tag HEAD as recovery point
ts=$(date +%s)
recovery_tag="cmupdate-pre-resync-$ts"
echo -n " [2/4] Creating recovery tag $recovery_tag  "
if ! git tag -f "$recovery_tag" HEAD >/dev/null 2>&1; then
  echo "FAIL"
  result_fail "tag" "Could not create recovery tag $recovery_tag."
fi
echo "OK"

# Step 3/4: stash with refs/stash sentinel so we only pop what we created.
stash_before=$(git rev-parse --verify refs/stash 2>/dev/null || echo none)
echo -n " [3/4] Stashing local changes (if any)...                 "
if git stash save -u "cmupdate-fix-stash-$ts" >/dev/null 2>&1; then
  stash_after=$(git rev-parse --verify refs/stash 2>/dev/null || echo none)
  if [ "$stash_after" != "$stash_before" ]; then
    echo "OK (slot stash@{0})"
    stash_created=1
  else
    echo "OK (no local changes — nothing to stash)"
    stash_created=0
  fi
else
  echo "skip"
  stash_created=0
fi

# Step 4/4: hard reset to remote tip
echo -n " [4/4] Resetting to origin/$branch...                "
reset_err=$(git reset --hard "origin/$branch" 2>&1 >/dev/null)
if [ $? -ne 0 ]; then
  echo "FAIL"
  result_fail "reset" "$(translate_git_error "$reset_err") Your recovery tag $recovery_tag still points at the old code."
fi
echo "OK"

chmod +x "$CM_INSTALLDIR/centmin.sh" 2>/dev/null

echo
bar
echo " RESULT: SUCCESS — Centmin Mod synced to $remote_sha"
bar
echo " Run  cmupdate  again to confirm everything works."
echo
echo " If something looks wrong after this update, roll back with:"
echo "   git -C $CM_INSTALLDIR reset --hard $recovery_tag"
echo
if [ "$stash_created" = "1" ]; then
  echo " To see your saved local changes:"
  echo "   git -C $CM_INSTALLDIR stash list"
  echo " To restore them:"
  echo "   git -C $CM_INSTALLDIR stash pop stash@{0}"
  echo
fi
echo " Help: $FORUM_URL"
bar
exit 0
