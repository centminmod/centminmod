#!/bin/bash
# Optional developer checks; never called by centmin.sh or its auto-updater.
# Exit 77 means the checks were skipped, not passed. No packages are installed.
wp_test_dir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd) || exit 1
for wp_test_python in python3 python3.{14..6} python36 python; do
  command -v "$wp_test_python" >/dev/null 2>&1 || continue
  if "$wp_test_python" -E -c 'import sys; sys.exit(sys.version_info < (3, 6))' >/dev/null 2>&1; then
    exec "$wp_test_python" -E "$wp_test_dir/test-wordpress-config.py" "$@"
  fi
done
echo 'SKIP: optional WordPress tests require Python 3.6+. Centmin Mod auto-updates do not use this test tool.' >&2
exit 77
