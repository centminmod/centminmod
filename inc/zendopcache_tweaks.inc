# for php 7 usage only
opcachehugepages() {
    FREEMEM=
    if [[ "$CENTOS_SIX" = '6' ]]; then
      if [[ ! -f /proc/user_beancounters && -f /usr/bin/numactl ]]; then
        # account for multiple cpu socket numa based memory
        # https://community.centminmod.com/posts/48189/
        GETCPUNODE_COUNT=$(numactl --hardware | awk '/available: / {print $2}')
        if [[ "$GETCPUNODE_COUNT" -ge '2' ]]; then
          FREEMEM_NUMANODE=$(numactl --hardware | awk '/free:/ {print $4}' | sort -r | head -n1)
          FREEMEMCACHED=$(grep -E '^Buffers|^Cached' /proc/meminfo | awk '{summ+=$2} END {print summ}' | head -n1)
          if [[ ! "$FREEMEM_NUMANODE" =~ ^[0-9]+$ || ! "$FREEMEMCACHED" =~ ^[0-9]+$ ]] || (( 10#$FREEMEM_NUMANODE <= 0 )); then
            # hugepages are an optional tuning; a bad probe must not stop the PHP build
            echo "WARNING: invalid NUMA available-memory probe; skipping PHP OPcache hugepages tuning." >&2
            return 0
          fi
          FREEMEM_NUMANODE=$((10#$FREEMEM_NUMANODE*1024))
          FREEMEM=$((FREEMEM_NUMANODE+10#$FREEMEMCACHED))
        else
          FREEMEM=$(grep -E '^MemFree|^Buffers|^Cached' /proc/meminfo | awk '{summ+=$2} END {print summ}' | head -n1)
        fi
      elif [[ -f /proc/user_beancounters ]]; then
        FREEMEMOPENVZ=$(grep '^MemFree' /proc/meminfo | awk '{summ+=$2} END {print summ}' | head -n1)
        FREEMEMCACHED=$(grep -E '^Buffers|^Cached' /proc/meminfo | awk '{summ+=$2} END {print summ}' | head -n1)
        FREEMEM=$(($FREEMEMOPENVZ+$FREEMEMCACHED))
      else
        FREEMEM=$(grep -E '^MemFree|^Buffers|^Cached' /proc/meminfo | awk '{summ+=$2} END {print summ}' | head -n1)
      fi
    elif [[ "$CENTOS_EIGHT" = '8' || "$CENTOS_SEVEN" = '7' || "$CENTOS_NINE" = '9' || "$CENTOS_TEN" = '10' ]]; then
      if [[ ! -f /proc/user_beancounters && -f /usr/bin/numactl ]]; then
        # account for multiple cpu socket numa based memory
        # https://community.centminmod.com/posts/48189/
        GETCPUNODE_COUNT=$(numactl --hardware | awk '/available: / {print $2}')
        if [[ "$GETCPUNODE_COUNT" -ge '2' ]]; then
          FREEMEM_NUMANODE=$(numactl --hardware | awk '/free:/ {print $4}' | sort -r | head -n1)
          FREEMEMCACHED=$(grep -E '^Buffers|^Cached' /proc/meminfo | awk '{summ+=$2} END {print summ}' | head -n1)
          if [[ ! "$FREEMEM_NUMANODE" =~ ^[0-9]+$ || ! "$FREEMEMCACHED" =~ ^[0-9]+$ ]] || (( 10#$FREEMEM_NUMANODE <= 0 )); then
            # hugepages are an optional tuning; a bad probe must not stop the PHP build
            echo "WARNING: invalid NUMA available-memory probe; skipping PHP OPcache hugepages tuning." >&2
            return 0
          fi
          FREEMEM_NUMANODE=$((10#$FREEMEM_NUMANODE*1024))
          FREEMEM=$((FREEMEM_NUMANODE+10#$FREEMEMCACHED))
        else
          FREEMEM=$(cat /proc/meminfo | grep MemAvailable | awk '{print $2}')
        fi
      elif [[ -f /proc/user_beancounters ]]; then
        FREEMEMOPENVZ=$(grep '^MemFree' /proc/meminfo | awk '{summ+=$2} END {print summ}' | head -n1)
        FREEMEMCACHED=$(grep -E '^Buffers|^Cached' /proc/meminfo | awk '{summ+=$2} END {print summ}' | head -n1)
        FREEMEM=$(($FREEMEMOPENVZ+$FREEMEMCACHED))
      else
        FREEMEM=$(cat /proc/meminfo | grep MemAvailable | awk '{print $2}')
      fi
    fi
    if [[ ! "$FREEMEM" =~ ^[0-9]+$ ]] || (( 10#$FREEMEM <= 0 )); then
        echo "WARNING: invalid available-memory probe (${FREEMEM:-empty}); skipping PHP OPcache hugepages tuning." >&2
        return 0
    fi
    FREEMEM=$((10#$FREEMEM))
    NRHUGEPAGES_COUNT=$(($FREEMEM/2/2048/16*16/4))
    MAXLOCKEDMEM_COUNT=$(($FREEMEM/2/2048/16*16*4))
    MAXLOCKEDMEM_SIZE=$(($MAXLOCKEDMEM_COUNT*1024))
    MAXLOCKEDMEM_SIZE_NGINX=$(($MAXLOCKEDMEM_SIZE*32))

    if [[ "$CENTOS_EIGHT" = '8' || "$CENTOS_SEVEN" = '7' ]] && [[ ! -f /proc/user_beancounters ]]; then
      echo
      echo "set vm.nr.hugepages in /etc/sysctl.conf"
      echo
      if [[ -z "$(grep '^vm.nr_hugepages' /etc/sysctl.conf)" ]]; then
        echo "vm.nr_hugepages=$NRHUGEPAGES_COUNT" >> /etc/sysctl.conf
        sysctl -p
        echo
      else
        sed -i "s|vm.nr_hugepages=.*|vm.nr_hugepages=$NRHUGEPAGES_COUNT|" /etc/sysctl.conf
        sysctl -p
        echo
      fi
    fi
    if [[ "$CHECK_LXD" != 'y' ]]; then
        echo
        echo "set system max locked memory limit"
        echo
        echo "/etc/security/limits.conf"
        echo "* soft memlock $MAXLOCKEDMEM_SIZE"
        echo "* hard memlock $MAXLOCKEDMEM_SIZE"
        sed -i '/hard memlock/d' /etc/security/limits.conf
        sed -i '/soft memlock/d' /etc/security/limits.conf
        if [[ -z "$(grep 'nginx soft memlock' /etc/security/limits.conf)" ]]; then
          echo "nginx soft memlock $MAXLOCKEDMEM_SIZE_NGINX" >> /etc/security/limits.conf
          echo "nginx hard memlock $MAXLOCKEDMEM_SIZE_NGINX" >> /etc/security/limits.conf
          echo
        fi
        if [[ -z "$(grep '* soft memlock' /etc/security/limits.conf)" ]]; then
          echo "* soft memlock $MAXLOCKEDMEM_SIZE" >> /etc/security/limits.conf
          echo "* hard memlock $MAXLOCKEDMEM_SIZE" >> /etc/security/limits.conf
          echo
        else
          sed -i "s|memlock .*|memlock $MAXLOCKEDMEM_SIZE|g" /etc/security/limits.conf
        fi
    fi

    cat /etc/security/limits.conf
    echo
    ulimit -H -l

  # check if redis installed as redis server requires huge pages disabled
  if [[ -f /usr/bin/redis-cli ]]; then
    if [[ -f /sys/kernel/mm/transparent_hugepage/enabled ]]; then
      echo never > /sys/kernel/mm/transparent_hugepage/enabled
      if [[ -z "$(grep transparent_hugepage /etc/rc.local)" ]]; then
        echo "echo never > /sys/kernel/mm/transparent_hugepage/enabled" >> /etc/rc.local
      fi
    fi
  fi

  # https://www.kernel.org/doc/Documentation/vm/transhuge.txt
  # only enable PHP zend opcache opcache.huge_code_pages=1 support if on CentOS 7.x and kernel
  # supports transparent hugepages. Otherwise, disable it in PHP zend opcache
  if [[ -f /sys/kernel/mm/transparent_hugepage/enabled ]]; then
    # cat /sys/kernel/mm/transparent_hugepage/enabled
    HP_CHECK=$(cat /sys/kernel/mm/transparent_hugepage/enabled | grep -o '\[.*\]')
    if [[ "$CENTOS_SIX" = '6' ]]; then
      if [ -f "${CONFIGSCANDIR}/zendopcache.ini" ]; then
        OPCACHEHUGEPAGES_OPT=''
        echo $OPCACHEHUGEPAGES_OPT
        if [[ "$(grep 'opcache.huge_code_pages' ${CONFIGSCANDIR}/zendopcache.ini)" ]]; then
          sed -i 's|^opcache.huge_code_pages=1|opcache.huge_code_pages=0|' ${CONFIGSCANDIR}/zendopcache.ini
          sed -i 's|^;opcache.huge_code_pages=1|opcache.huge_code_pages=0|' ${CONFIGSCANDIR}/zendopcache.ini
        else
          echo -e "\nopcache.huge_code_pages=0" >> ${CONFIGSCANDIR}/zendopcache.ini
        fi
      fi      
    elif [[ "$CENTOS_EIGHT" = '8' || "$CENTOS_SEVEN" = '7' ]] && [[ "$HP_CHECK" = '[always]' ]]; then
      if [ -f "${CONFIGSCANDIR}/zendopcache.ini" ]; then
        OPCACHEHUGEPAGES_OPT=' --enable-huge-code-pages'
        echo $OPCACHEHUGEPAGES_OPT
        if [ -f "${CM_INSTALLDIR}/tools/hptweaks.sh" ]; then
          $CM_INSTALLDIR/tools/hptweaks.sh
        fi
        if [[ "$(grep 'opcache.huge_code_pages' ${CONFIGSCANDIR}/zendopcache.ini)" ]]; then
          sed -i 's|^;opcache.huge_code_pages=1|opcache.huge_code_pages=1|' ${CONFIGSCANDIR}/zendopcache.ini
        else
          echo -e "\nopcache.huge_code_pages=1" >> ${CONFIGSCANDIR}/zendopcache.ini
        fi
      fi
    elif [[ "$CENTOS_EIGHT" = '8' || "$CENTOS_SEVEN" = '7' ]] && [[ "$HP_CHECK" = '[never]' && "$PHP_HUGEPAGES" = [yY] && ! -f /proc/user_beancounters && "$FREEMEM" -ge '1980000' ]]; then
      # setting PHP explicit huge pages support for zend opcache
      # opcache.huge_code_pages
      if [ -f "${CONFIGSCANDIR}/zendopcache.ini" ]; then
        OPCACHEHUGEPAGES_OPT=' --enable-huge-code-pages'
        echo $OPCACHEHUGEPAGES_OPT
        if [[ "$(grep 'opcache.huge_code_pages' ${CONFIGSCANDIR}/zendopcache.ini)" ]]; then
          sed -i 's|^opcache.huge_code_pages=.*|opcache.huge_code_pages=1|' ${CONFIGSCANDIR}/zendopcache.ini
          sed -i 's|^;opcache.huge_code_pages=.*|opcache.huge_code_pages=1|' ${CONFIGSCANDIR}/zendopcache.ini
        else
          echo -e "\nopcache.huge_code_pages=1" >> ${CONFIGSCANDIR}/zendopcache.ini
        fi
        echo
        echo "sysctl vm.nr_hugepages"
        sysctl vm.nr_hugepages
        echo
        cat /proc/meminfo
        echo
        php --ri 'zend opcache' | grep 'opcache.huge_code_pages'
        echo
      fi
    elif [[ "$CENTOS_SEVEN" = '7' && "$HP_CHECK" = '[never]' && "$PHP_HUGEPAGES" = [nN] ]] || [[ "$CENTOS_SEVEN" = '7' && "$HP_CHECK" = '[never]' && "$PHP_HUGEPAGES" = [yY] && -f /proc/user_beancounters ]]; then
      # setting PHP explicit huge pages support for zend opcache
      # opcache.huge_code_pages
      if [ -f "${CONFIGSCANDIR}/zendopcache.ini" ]; then
        OPCACHEHUGEPAGES_OPT=''
        echo $OPCACHEHUGEPAGES_OPT
        if [[ "$(grep 'opcache.huge_code_pages' ${CONFIGSCANDIR}/zendopcache.ini)" ]]; then
          sed -i 's|^opcache.huge_code_pages=.*|opcache.huge_code_pages=0|' ${CONFIGSCANDIR}/zendopcache.ini
          sed -i 's|^;opcache.huge_code_pages=.*|opcache.huge_code_pages=0|' ${CONFIGSCANDIR}/zendopcache.ini
        else
          echo -e "\nopcache.huge_code_pages=0" >> ${CONFIGSCANDIR}/zendopcache.ini
        fi
        echo
        echo "sysctl vm.nr_hugepages"
        sysctl vm.nr_hugepages
        echo
        cat /proc/meminfo
        echo
        php --ri 'zend opcache' | grep 'opcache.huge_code_pages'
        echo
      fi        
    elif [[ "$CENTOS_EIGHT" = '8' && "$HP_CHECK" = '[never]' && "$PHP_HUGEPAGES" = [nN] ]] || [[ "$CENTOS_EIGHT" = '8' && "$HP_CHECK" = '[never]' && "$PHP_HUGEPAGES" = [yY] && -f /proc/user_beancounters ]]; then
      # setting PHP explicit huge pages support for zend opcache
      # opcache.huge_code_pages
      if [ -f "${CONFIGSCANDIR}/zendopcache.ini" ]; then
        OPCACHEHUGEPAGES_OPT=''
        echo $OPCACHEHUGEPAGES_OPT
        if [[ "$(grep 'opcache.huge_code_pages' ${CONFIGSCANDIR}/zendopcache.ini)" ]]; then
          sed -i 's|^opcache.huge_code_pages=.*|opcache.huge_code_pages=0|' ${CONFIGSCANDIR}/zendopcache.ini
          sed -i 's|^;opcache.huge_code_pages=.*|opcache.huge_code_pages=0|' ${CONFIGSCANDIR}/zendopcache.ini
        else
          echo -e "\nopcache.huge_code_pages=0" >> ${CONFIGSCANDIR}/zendopcache.ini
        fi
        echo
        echo "sysctl vm.nr_hugepages"
        sysctl vm.nr_hugepages
        echo
        cat /proc/meminfo
        echo
        php --ri 'zend opcache' | grep 'opcache.huge_code_pages'
        echo
      fi        
    fi
  else
    if [ -f "${CONFIGSCANDIR}/zendopcache.ini" ]; then
      if [[ "$(grep 'opcache.huge_code_pages' ${CONFIGSCANDIR}/zendopcache.ini)" ]]; then
        sed -i 's|^opcache.huge_code_pages=1|opcache.huge_code_pages=0|' ${CONFIGSCANDIR}/zendopcache.ini
        sed -i 's|^;opcache.huge_code_pages=1|opcache.huge_code_pages=0|' ${CONFIGSCANDIR}/zendopcache.ini
      else
        echo -e "\nopcache.huge_code_pages=0" >> ${CONFIGSCANDIR}/zendopcache.ini
      fi    
    fi
  fi
}
# PHP_OPCACHE_JIT='y' (PHP 8.0+) and PHP_HUGEPAGES='y' (EL9/EL10) write the
# Centmin Mod managed ${CONFIGSCANDIR}/zz-opcache-tuning.ini. It sorts after
# zendopcache.ini, so its values win, and zendopcache.ini rewrites (menu 5,
# menu 12) leave it alone. It is removed when neither toggle applies.
# Measured 2026-11-18 on XenForo 2.3, PHP 8.3, Xeon E3-1270 v6: tracing JIT
# with a 64M buffer +10-12% req/s, huge pages +4%, both +16-18%.
# EL9/EL10 huge pages: vm.nr_hugepages is sized to the OPcache segment
# (opcache.memory_consumption + JIT buffer, in 2 MB pages, + 32 for PHP's code
# copied by huge_code_pages), capped at a quarter of available memory, and kept
# in /etc/sysctl.d/102-php-opcache-hugepages.conf. PHP 8 maps the segment with
# MAP_HUGETLB in one piece when enough pages are free, else it quietly uses
# normal pages, so no pages are reserved when the segment can't use them: a
# segment size that is not a multiple of 2 MB, a default huge page size other
# than 2 MB, or a segment larger than a quarter of available memory. A JIT
# buffer set in another ini file (PHP_OPCACHE_JIT='n') is counted too. LXD/LXC
# containers are skipped, like the EL7/EL8 huge pages setup above.
# A pool sized outside Centmin Mod (vm.nr_hugepages already set) is left alone.
# Called after phpsededit on installs and menu 5 PHP upgrades, after the PHP
# build (PHP extension builds can still follow; the 2 GB minimum leaves them room).
# Restart PHP-FPM after a change (fpmrestart): a reload starts the new master
# while the old OPcache segment still holds its huge pages.
opcache_tuning_ini() {
  local ini="${CONFIGSCANDIR}/zz-opcache-tuning.ini"
  local hpconf=/etc/sysctl.d/102-php-opcache-hugepages.conf
  local phpver="${PHPMUVER:-$PHPMVER}" phpmajor jitbuf="${PHP_OPCACHE_JIT_BUFFER:-64M}"
  local lines=() jitmb=0 segjit hpwant=0 opmem need pages maxpages availkb hptotal hpsizekb f
  [[ -n "$CONFIGSCANDIR" && -d "$CONFIGSCANDIR" ]] || return 0
  phpmajor="${phpver%%.*}"
  if [[ "$PHP_OPCACHE_JIT" = [yY] && "$phpmajor" =~ ^[0-9]+$ && "$phpmajor" -ge 8 ]]; then
    if [[ "$jitbuf" =~ ^([0-9]+)[Mm]$ ]]; then
      jitmb="${BASH_REMATCH[1]}"
    elif [[ "$jitbuf" =~ ^([0-9]+)[Gg]$ ]]; then
      jitmb=$((BASH_REMATCH[1] * 1024))
    else
      echo "PHP_OPCACHE_JIT_BUFFER='$jitbuf' not understood (use e.g. 64M), using 64M"
      jitbuf='64M'; jitmb=64
    fi
    lines+=("opcache.jit=tracing" "opcache.jit_buffer_size=${jitbuf}")
  fi
  if [[ "$CENTOS_NINE" = '9' || "$CENTOS_TEN" = '10' ]]; then
    availkb=$(awk '/^MemAvailable:/ {print $2}' /proc/meminfo)
    # the configured pool; HugePages_Total also counts in-use surplus pages left
    # after the pool was set to 0, until PHP-FPM restarts
    hptotal=$(cat /proc/sys/vm/nr_hugepages 2>/dev/null)
    hpsizekb=$(awk '/^Hugepagesize:/ {print $2}' /proc/meminfo)
    [[ "$hptotal" =~ ^[0-9]+$ ]] || hptotal=0
    [[ "$hpsizekb" =~ ^[0-9]+$ ]] || hpsizekb=2048
    # Pages reserved by an earlier run no longer count as available memory: add
    # them back, so every run sizes and checks against the same memory.
    [[ -f "$hpconf" && "$availkb" =~ ^[0-9]+$ ]] && availkb=$(( availkb + hptotal * hpsizekb ))
    if [[ "$PHP_HUGEPAGES" = [yY] && ! -f /proc/user_beancounters && "$CHECK_LXD" != [yY] ]]; then
      # the last opcache.memory_consumption in the scanned ini files (PHP reads them
      # in this order, so a later file wins); first number on the line, so a
      # trailing "; comment 256" can't be folded in
      opmem=$(cat "${CONFIGSCANDIR}"/*.ini 2>/dev/null | awk -F'=' '/^[[:space:]]*opcache\.memory_consumption[[:space:]]*=/ {if (match($2, /[0-9]+/)) v=substr($2, RSTART, RLENGTH)} END {print v}')
      # the JIT buffer is part of the same segment: PHP_OPCACHE_JIT's, else one set in
      # another ini file (the last opcache.jit_buffer_size: bytes, K, M or G)
      segjit=$jitmb
      if [[ "$segjit" -eq 0 ]]; then
        segjit=$(for f in "${CONFIGSCANDIR}"/*.ini; do [[ "$f" = "$ini" ]] || cat "$f"; done 2>/dev/null \
          | awk -F'=' '/^[[:space:]]*opcache\.jit_buffer_size[[:space:]]*=/ {v=$2} END {sub(/;.*/, "", v); gsub(/[[:space:]"\047]/, "", v); u=toupper(substr(v, length(v))); n=v+0; if (u=="G") n*=1024; else if (u=="K") n=int(n/1024); else if (u!="M") n=int(n/1048576); print n+0}')
        [[ "$segjit" =~ ^[0-9]+$ ]] || segjit=0
      fi
      [[ "$opmem" =~ ^[0-9]+$ ]] && need=$(( (opmem + segjit) / 2 ))
      if [[ ! -f "$hpconf" && "$hptotal" -gt 0 ]]; then
        # a pool set outside Centmin Mod (sysctl, kernel command line, tuned): leave its
        # size alone; OPcache uses it when it is large enough
        lines+=("opcache.huge_code_pages=1")
        echo "PHP_HUGEPAGES: vm.nr_hugepages is already ${hptotal}, set outside Centmin Mod; pool size left as it is"
      elif [[ -z "$need" ]]; then
        echo "PHP_HUGEPAGES='y' but no opcache.memory_consumption in ${CONFIGSCANDIR}: no huge pages reserved"
      elif [[ "$hpsizekb" -ne 2048 ]]; then
        echo "PHP_HUGEPAGES='y' but the default huge page size is ${hpsizekb} kB, and PHP OPcache needs 2 MB pages: no huge pages reserved"
      elif (( opmem % 2 || segjit % 2 )); then
        # PHP maps the segment on huge pages only when its size is a 2 MB multiple,
        # and the JIT buffer, at the memory_consumption offset, must start on a 2 MB
        # boundary or JIT can't make it executable
        echo "PHP_HUGEPAGES='y' but OPcache ${opmem}M and JIT ${segjit}M are not both multiples of 2 MB: no huge pages reserved"
      elif [[ ! "$availkb" =~ ^[0-9]+$ || "$availkb" -lt '1980000' ]]; then
        echo "PHP_HUGEPAGES='y' but under 2 GB of memory is available: no huge pages reserved"
      elif (( need > availkb / 4 / hpsizekb )); then
        echo "PHP_HUGEPAGES='y' but OPcache ${opmem}M + JIT ${segjit}M is more than a quarter of available memory: no huge pages reserved"
      else
        hpwant=1
      fi
    fi
    if [[ "$hpwant" -eq 1 ]]; then
      pages=$(( need + 32 ))
      maxpages=$(( availkb / 4 / hpsizekb ))
      (( pages > maxpages )) && pages=$maxpages
      printf '%s\n' "# centminmod PHP_HUGEPAGES='y' (EL9/EL10): OPcache ${opmem}M + JIT ${segjit}M + PHP code" \
        "vm.nr_hugepages = ${pages}" > "$hpconf"
      sysctl -q -w vm.nr_hugepages="${pages}" >/dev/null 2>&1
      lines+=("opcache.huge_code_pages=1")
      hptotal=$(cat /proc/sys/vm/nr_hugepages 2>/dev/null)
      echo "vm.nr_hugepages = ${pages} ($hpconf), allocated now: ${hptotal}"
      if [[ "$hptotal" =~ ^[0-9]+$ && "$hptotal" -lt "$need" ]]; then
        echo "note: OPcache needs ${need} huge pages but only ${hptotal} could be allocated now (fragmented memory); the full pool is allocated at the next boot"
      fi
      if grep -qsE '^[[:space:]]*vm\.nr_hugepages[[:space:]]*=' /etc/sysctl.conf; then
        echo "note: /etc/sysctl.conf also sets vm.nr_hugepages and is applied after $hpconf at boot"
      fi
    elif [[ -f "$hpconf" ]]; then
      rm -f "$hpconf"
      if ! grep -qsE '^[[:space:]]*vm\.nr_hugepages[[:space:]]*=' /etc/sysctl.conf /etc/sysctl.d/*.conf /run/sysctl.d/*.conf /usr/local/lib/sysctl.d/*.conf /usr/lib/sysctl.d/*.conf \
        && ! grep -q 'hugepages=' /proc/cmdline 2>/dev/null; then
        sysctl -q -w vm.nr_hugepages=0 >/dev/null 2>&1
      fi
      echo "PHP OPcache huge pages no longer reserved: removed $hpconf"
    fi
  fi
  if [[ "${#lines[@]}" -gt 0 ]]; then
    printf '%s\n' "; managed by Centmin Mod (PHP_OPCACHE_JIT / PHP_HUGEPAGES), rewritten on PHP installs and upgrades" "${lines[@]}" > "$ini"
    echo "$ini:"; sed 's/^/  /' "$ini"
  elif [[ -f "$ini" ]]; then
    rm -f "$ini"
    echo "PHP_OPCACHE_JIT and PHP_HUGEPAGES not enabled: removed $ini"
  fi
  return 0
}
