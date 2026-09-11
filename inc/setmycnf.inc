# Initial-install only. Upgrade/version cleanup remains in the version installers.
# Optional persistent settings in /etc/centminmod/custom_config.inc:
# MYSQL_MEMORY_TEMP_TABLES=large doubles the 2/4/8 GB temp limits.
# MYSQL_<UPPERCASE_OPTION> overrides a supported memory option, e.g.
# MYSQL_INNODB_BUFFER_POOL_SIZE=512M or MYSQL_TMP_TABLE_SIZE=128M.
# A tmp-table override also sets max_heap_table_size unless explicitly overridden.

mysql_memory_capacity() {
  local proc=${1:-/proc} os_release=${2:-/etc/os-release} host effective os_id os_version
  local hierarchy controllers group root mount dir limit
  host=$(awk '$1 == "MemTotal:" {print $2; exit}' "$proc/meminfo") || return 1
  [[ "$host" =~ ^[1-9][0-9]*$ ]] || return 1
  # Disabled unless explicitly enabled in pre-install custom_config.inc.
  if [[ "${MYSQL_CGROUP_DETECTION:-n}" != [yY] ]]; then
    printf '%s %s\n' "$host" "$host"; return 0
  fi
  # 141 only: opt in known OS releases to read-only cgroup v2 detection.
  if [[ -r "$os_release" ]]; then
    read -r os_id os_version < <(awk -F= '
      $1 == "ID" {id=$2} $1 == "VERSION_ID" {version=$2}
      END {gsub(/\042|\047/, "", id); gsub(/\042|\047/, "", version); print id, version}
    ' "$os_release")
  fi
  case "$os_id:${os_version%%.*}" in
    almalinux:9|almalinux:10) ;;
    *) printf '%s %s\n' "$host" "$host"; return 0 ;;
  esac
  effective=$host
  # Inspect only mounted v2 memory limits and visible ancestors.
  # This is the installer's cgroup, not a prediction of future service overrides.
  if [[ -r "$proc/self/cgroup" && -r "$proc/self/mountinfo" ]]; then
    while IFS=: read -r hierarchy controllers group; do
      [[ "$hierarchy" == 0 && -z "$controllers" ]] || continue
      while read -r root mount; do
        root=$(printf '%b' "$root"); mount=$(printf '%b' "$mount")
        if [[ "$root" == / ]]; then
          dir="${mount}${group%/}"
        elif [[ "$group" == "$root" || "$group" == "$root/"* ]]; then
          dir="${mount}${group#"$root"}"
        elif [[ "$group" == / ]]; then
          dir=$mount
        else
          continue
        fi
        # Kernel paths normally have no traversal components; do not escape a mount.
        [[ "$dir" != */../* && "$dir" != */.. ]] || continue
        while [[ "$dir" == "$mount" || "$dir" == "$mount/"* ]]; do
          if [[ -r "$dir/memory.max" ]]; then
            limit=$(cat "$dir/memory.max") || return 1
            if [[ "$limit" != max ]]; then
              [[ "$limit" =~ ^[0-9]+$ ]] || return 1
              effective=$(awk -v cap="$effective" -v bytes="$limit" 'BEGIN {
                if (bytes < cap * 1024) cap = int(bytes / 1024)
                printf "%.0f\n", cap
              }') || return 1
            fi
          fi
          [[ "$dir" == "$mount" ]] && break
          dir=${dir%/*}
        done
      done < <(awk '
        {for (i=7; i<=NF; i++) if ($i == "-") {
          if ($(i+1) == "cgroup2")
            print $4, $5
          break
        }}' "$proc/self/mountinfo")
    done < "$proc/self/cgroup"
  fi
  [[ "$effective" =~ ^[1-9][0-9]*$ ]] || return 1
  printf '%s %s\n' "$host" "$effective"
}

mysql_memory_profile() {
  local memory=$1
  [[ "$memory" =~ ^[1-9][0-9]*$ && ${#memory} -le 12 ]] || return 1
  MYCNF_MEMORY_SETTINGS='query_cache_type=0 query_cache_size=0'
  if (( memory < 1572864 )); then
    MYCNF_PROFILE=minimum; MYCNF_TEMPLATE=my-mdb10-min.cnf
  elif (( memory < 3145728 )); then
    MYCNF_PROFILE=2gb; MYCNF_TEMPLATE=my-mdb10-min.cnf
    MYCNF_MEMORY_SETTINGS+=' innodb_buffer_pool_size=128M aria_pagecache_buffer_size=16M aria_sort_buffer_size=16M'
  elif (( memory < 6291456 )); then
    MYCNF_PROFILE=4gb; MYCNF_TEMPLATE=my-mdb10.cnf
    MYCNF_MEMORY_SETTINGS+=' innodb_buffer_pool_size=256M tmp_table_size=64M max_heap_table_size=64M'
  elif (( memory < 12582912 )); then
    MYCNF_PROFILE=8gb; MYCNF_TEMPLATE=my-mdb10-4gb.cnf
    MYCNF_MEMORY_SETTINGS+=' key_buffer_size=64M tmp_table_size=128M max_heap_table_size=128M'
  elif (( memory < 25165824 )); then
    MYCNF_PROFILE=16gb; MYCNF_TEMPLATE=my-mdb10-16gb.cnf
  elif (( memory < 50331648 )); then
    MYCNF_PROFILE=32gb; MYCNF_TEMPLATE=my-mdb10-32gb.cnf
  elif (( memory < 100663296 )); then
    MYCNF_PROFILE=64gb; MYCNF_TEMPLATE=my-mdb10-64gb-8c.cnf
  else
    # Large systems retain the current cache budget until workload validation.
    MYCNF_PROFILE=large; MYCNF_TEMPLATE=my-mdb10-64gb-8c.cnf
  fi
  case "${MYSQL_MEMORY_TEMP_TABLES:-default}" in
    default) ;;
    large)
      case "$MYCNF_PROFILE" in
        2gb) MYCNF_MEMORY_SETTINGS+=' tmp_table_size=64M max_heap_table_size=64M' ;;
        4gb) MYCNF_MEMORY_SETTINGS+=' tmp_table_size=128M max_heap_table_size=128M' ;;
        8gb) MYCNF_MEMORY_SETTINGS+=' tmp_table_size=256M max_heap_table_size=256M' ;;
      esac ;;
    *) echo 'Invalid MYSQL_MEMORY_TEMP_TABLES: use default or large' >&2; return 1 ;;
  esac
}

mysql_memory_render() {
  local template=$1 name variable value overrides=$MYCNF_MEMORY_SETTINGS
  for name in innodb_buffer_pool_size key_buffer_size aria_pagecache_buffer_size \
    aria_sort_buffer_size myisam_sort_buffer_size join_buffer_size read_buffer_size \
    sort_buffer_size read_rnd_buffer_size tmp_table_size max_heap_table_size \
    max_connections thread_cache_size query_cache_type query_cache_size; do
    variable=MYSQL_$(printf '%s' "$name" | tr '[:lower:]' '[:upper:]')
    [[ ${!variable+x} ]] || continue
    value=${!variable}
    case "$name" in
      max_connections)
        [[ "$value" =~ ^[1-9][0-9]*$ && ${#value} -le 6 ]] && (( value <= 100000 )) || return 1 ;;
      thread_cache_size) [[ "$value" =~ ^[0-9]+$ && ${#value} -le 5 ]] || return 1 ;;
      query_cache_type) [[ "$value" =~ ^[012]$ ]] || return 1 ;;
      *) [[ "$value" =~ ^[0-9]+[KkMmGg]?$ && ${#value} -le 16 ]] || return 1 ;;
    esac
    overrides+=" $name=$value"
    if [[ "$name" == tmp_table_size && ! ${MYSQL_MAX_HEAP_TABLE_SIZE+x} ]]; then
      overrides+=" max_heap_table_size=$value"
    fi
  done
  # Preserve option groups and all unrelated/version-specific template settings.
  awk -v overrides="$overrides" '
    BEGIN {n=split(overrides, pairs, " "); for (i=1;i<=n;i++) {
      split(pairs[i], pair, "="); values[pair[1]]=pair[2]
    }}
    /^\[/ {server=($0 ~ /^\[(mysqld|mariadb)\][[:space:]]*$/)}
    server && /^[[:space:]]*[a-z_]+[[:space:]]*=/ {
      key=$0; sub(/=.*/, "", key); gsub(/[[:space:]]/, "", key)
      if (key in values) {print key " = " values[key]; seen[key]++; next}
    }
    {print}
    END {for (key in values) if (seen[key] != 1) {
      print "Expected one template setting: " key > "/dev/stderr"; failed=1
    }; if (failed) exit 1}
  ' "$template"
}

mysql_memory_backup() {
  if [[ -d /etc/my.cnf || -d /etc/my.cnf-original || -L /etc/my.cnf-original ]]; then
    echo 'Refusing non-file my.cnf destination or unsafe backup destination' >&2
    return 1
  fi
  # Fresh-install wrappers call this before removing/replacing RPM packages.
  # Do not overwrite that backup with a package-generated config later on.
  [[ "${_CMM_MYCNF_BACKED_UP:-n}" == y ]] && return 0
  if [[ -f /etc/my.cnf ]]; then
    cp -p /etc/my.cnf /etc/my.cnf-original || return 1
  fi
  _CMM_MYCNF_BACKED_UP=y
}

setmycnf() {
  local capacity host memory staged log
  capacity=$(mysql_memory_capacity) || { echo 'Unable to determine MariaDB memory capacity' >&2; exit 1; }
  read -r host memory <<< "$capacity"
  mysql_memory_profile "$memory" || { echo 'Invalid MariaDB memory profile settings' >&2; exit 1; }
  staged=$(mktemp /etc/my.cnf.centmin.XXXXXX) || exit 1
  if ! mysql_memory_render "$CUR_DIR/config/mysql/$MYCNF_TEMPLATE" > "$staged"; then
    rm -f "$staged"
    echo 'Unable to generate my.cnf; check MYSQL_* overrides and the template' >&2
    exit 1
  fi
  # Keep the previous file until the entire replacement has been rendered.
  if ! mysql_memory_backup; then
    rm -f "$staged"; exit 1
  fi
  if ! chmod 644 "$staged" || ! mv -f "$staged" /etc/my.cnf; then
    rm -f "$staged"; exit 1
  fi
  unset _CMM_MYCNF_BACKED_UP
  mysql_charset_default
  log="${CENTMINLOGDIR}/setmycnf_${SCRIPT_VERSION}_${DT}.log"
  {
    echo "MariaDB initial profile=$MYCNF_PROFILE host_kib=$host effective_kib=$memory template=$MYCNF_TEMPLATE temp_policy=${MYSQL_MEMORY_TEMP_TABLES:-default} cgroup_detection=${MYSQL_CGROUP_DETECTION:-n}"
    cat /etc/my.cnf
  } | tee "$log"
}
