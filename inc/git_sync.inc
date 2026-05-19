#!/bin/bash
######################################################
# inc/git_sync.inc
# Rebase-aware sync helper for Centmin Mod's git
# checkout at /usr/local/src/centminmod.
#
# Exported:
#   _cmm_git_sync_impl <branchname> [<workdir>]
#
# Return codes:
#   0 = updated (or already current) — caller may run post-update steps
#   1 = nothing to do / skipped — caller should treat as no-op
#   2 = error — caller should surface to user
#
# Behavior:
#   - Fast-forward case: stash (with refs/stash sentinel), pull --ff-only,
#     pop only the stash entry we created.
#   - Local-ahead case: warn, skip (return 1) — never rewinds local commits.
#   - Diverged case (upstream rebase / force-push): tag HEAD as
#     cmupdate-pre-resync-<epoch>, save dirty WT to stash (sentinel-checked),
#     hard reset to origin/<branch>. Refuses to reset if stash failed AND
#     working tree is dirty.
#
# EL7 git 1.8.3.1 compatible:
#   - Uses git stash save -u (legacy syntax), not git stash push.
#   - Uses git merge-base --is-ancestor (git 1.8.0+).
######################################################

_cmm_git_sync_impl() {
  local branch="$1"
  local workdir="${2:-/usr/local/src/centminmod}"
  local forum_url='https://community.centminmod.com/forums/install-upgrades-or-pre-install-questions.8/'

  if [ -z "$branch" ]; then
    echo "git_sync: ERROR — no branch name supplied"
    return 2
  fi
  if [ ! -d "$workdir/.git" ]; then
    echo "git_sync: ERROR — $workdir/.git not found (not a git checkout)"
    return 2
  fi

  pushd "$workdir" >/dev/null 2>&1 || { echo "git_sync: ERROR — could not enter $workdir"; return 2; }

  if ! git fetch --prune origin "$branch" 2>&1; then
    echo "git_sync: ERROR — git fetch failed for origin/$branch (network or auth)"
    popd >/dev/null 2>&1
    return 2
  fi

  if ! git rev-parse --verify --quiet "origin/$branch" >/dev/null; then
    echo "git_sync: ERROR — branch '$branch' not found on remote (renamed or deleted?)"
    popd >/dev/null 2>&1
    return 2
  fi

  local local_sha remote_sha
  local_sha=$(git rev-parse HEAD)
  remote_sha=$(git rev-parse "origin/$branch")

  if [ "$local_sha" = "$remote_sha" ]; then
    popd >/dev/null 2>&1
    return 1
  fi

  if git merge-base --is-ancestor HEAD "origin/$branch" 2>/dev/null; then
    # ---- FAST-FORWARD CASE ----
    local stash_before stash_after stash_rc
    stash_before=$(git rev-parse --verify refs/stash 2>/dev/null || echo none)
    git stash save -u "cmupdate-autostash-$(date +%s)" >/dev/null 2>&1
    stash_rc=$?

    if ! git pull --ff-only origin "$branch"; then
      echo "git_sync: ERROR — git pull --ff-only failed unexpectedly after merge-base check"
      popd >/dev/null 2>&1
      return 2
    fi

    if [ "$stash_rc" = "0" ]; then
      stash_after=$(git rev-parse --verify refs/stash 2>/dev/null || echo none)
      if [ "$stash_after" != "$stash_before" ]; then
        if ! git stash pop --quiet 2>/dev/null; then
          echo "git_sync: WARNING — local changes could not be re-applied cleanly"
          echo "  Your original changes are preserved in 'git -C $workdir stash list'"
          echo "  Resolve any <<<<<< conflict markers manually, then 'git stash drop' when done."
        fi
      fi
    fi
    popd >/dev/null 2>&1
    return 0
  fi

  if git merge-base --is-ancestor "origin/$branch" HEAD 2>/dev/null; then
    # ---- LOCAL-AHEAD CASE ----
    echo "git_sync: local HEAD is ahead of origin/$branch — refusing to rewind. Skipping update."
    popd >/dev/null 2>&1
    return 1
  fi

  # ---- DIVERGED CASE (upstream rebase / force-push) ----
  local ts recovery_tag dirty stash_before stash_after stash_rc stash_created
  ts=$(date +%s)
  recovery_tag="cmupdate-pre-resync-$ts"

  dirty=$(git status --porcelain 2>/dev/null)

  if ! git tag -f "$recovery_tag" HEAD >/dev/null 2>&1; then
    echo "git_sync: ERROR — could not create recovery tag $recovery_tag (refusing to reset without one)"
    popd >/dev/null 2>&1
    return 2
  fi

  stash_before=$(git rev-parse --verify refs/stash 2>/dev/null || echo none)
  git stash save -u "cmupdate-pre-resync-$ts" >/dev/null 2>&1
  stash_rc=$?
  stash_after=$(git rev-parse --verify refs/stash 2>/dev/null || echo none)

  if [ "$stash_rc" != "0" ] && [ -n "$dirty" ]; then
    echo "git_sync: ERROR — could not save local changes to a stash"
    echo "  Working tree is dirty. Refusing to hard-reset and lose your changes."
    echo "  Resolve with:  cd $workdir && git status   (review/commit/discard changes)"
    echo "  Then re-run cmupdate. Recovery tag: $recovery_tag"
    echo "  Help: $forum_url"
    popd >/dev/null 2>&1
    return 2
  fi

  if [ "$stash_after" != "$stash_before" ]; then
    stash_created=1
  else
    stash_created=0
  fi

  if ! git reset --hard "origin/$branch"; then
    echo "git_sync: ERROR — git reset --hard origin/$branch failed"
    echo "  Recovery tag $recovery_tag still points at your previous code."
    if [ "$stash_created" = "1" ]; then
      echo "  Local changes preserved in: git -C $workdir stash list"
    fi
    popd >/dev/null 2>&1
    return 2
  fi

  chmod +x centmin.sh 2>/dev/null

  echo
  echo "  GitHub rewrote the $branch history (rebase / force-push)."
  echo "  Centmin Mod local code has been resynced to origin/$branch."
  echo "  Recovery tag: $recovery_tag"
  if [ "$stash_created" = "1" ]; then
    echo "  Local changes saved to: git -C $workdir stash list"
  fi
  echo "  Roll back with: git -C $workdir reset --hard $recovery_tag"
  echo

  popd >/dev/null 2>&1
  return 0
}
