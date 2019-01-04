# for php 7 usage only
opcachehugepages() {
    if [[ "$CENTOS_SIX" = '6' ]]; then
      if [[ ! -f /proc/user_beancounters && -f /usr/bin/numactl ]]; then
        # account for multiple cpu socket numa based memory
        # https://community.centminmod.com/posts/48189/
        GETCPUNODE_COUNT=$(numactl --hardware | awk '/available: / {print $2}')
        if [[ "$GETCPUNODE_COUNT" -ge '2' ]]; then
          FREEMEM_NUMANODE=$(($(numactl --hardware | awk '/free:/ {print $4}' | sort -r | head -n1)*1024))
          FREEMEMCACHED=$(egrep '^Buffers|^Cached' /proc/meminfo | awk '{summ+=$2} END {print summ}' | head -n1)
          FREEMEM=$(($FREEMEM_NUMANODE+$FREEMEMCACHED))
        else
          FREEMEM=$(egrep '^MemFree|^Buffers|^Cached' /proc/meminfo | awk '{summ+=$2} END {print summ}' | head -n1)
        fi
      elif [[ -f /proc/user_beancounters ]]; then
        FREEMEMOPENVZ=$(grep '^MemFree' /proc/meminfo | awk '{summ+=$2} END {print summ}' | head -n1)
        FREEMEMCACHED=$(egrep '^Buffers|^Cached' /proc/meminfo | awk '{summ+=$2} END {print summ}' | head -n1)
        FREEMEM=$(($FREEMEMOPENVZ+$FREEMEMCACHED))
      else
        FREEMEM=$(egrep '^MemFree|^Buffers|^Cached' /proc/meminfo | awk '{summ+=$2} END {print summ}' | head -n1)
      fi
    elif [[ "$CENTOS_SEVEN" = '7' ]]; then
      if [[ ! -f /proc/user_beancounters && -f /usr/bin/numactl ]]; then
        # account for multiple cpu socket numa based memory
        # https://community.centminmod.com/posts/48189/
        GETCPUNODE_COUNT=$(numactl --hardware | awk '/available: / {print $2}')
        if [[ "$GETCPUNODE_COUNT" -ge '2' ]]; then
          FREEMEM_NUMANODE=$(($(numactl --hardware | awk '/free:/ {print $4}' | sort -r | head -n1)*1024))
          FREEMEMCACHED=$(egrep '^Buffers|^Cached' /proc/meminfo | awk '{summ+=$2} END {print summ}' | head -n1)
          FREEMEM=$(($FREEMEM_NUMANODE+$FREEMEMCACHED))
        else
          FREEMEM=$(cat /proc/meminfo | grep MemAvailable | awk '{print $2}')
        fi
      elif [[ -f /proc/user_beancounters ]]; then
        FREEMEMOPENVZ=$(grep '^MemFree' /proc/meminfo | awk '{summ+=$2} END {print summ}' | head -n1)
        FREEMEMCACHED=$(egrep '^Buffers|^Cached' /proc/meminfo | awk '{summ+=$2} END {print summ}' | head -n1)
        FREEMEM=$(($FREEMEMOPENVZ+$FREEMEMCACHED))
      else
        FREEMEM=$(cat /proc/meminfo | grep MemAvailable | awk '{print $2}')
      fi
    fi
    NRHUGEPAGES_COUNT=$(($FREEMEM/2/2048/16*16/4))
    MAXLOCKEDMEM_COUNT=$(($FREEMEM/2/2048/16*16*4))
    MAXLOCKEDMEM_SIZE=$((MAXLOCKEDMEM_COUNT*1024))

    if [[ "$CENTOS_SEVEN" = '7' && ! -f /proc/user_beancounters ]]; then
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
    echo
    echo "set system max locked memory limit"
    echo
    echo "/etc/security/limits.conf"
    echo "* soft memlock $MAXLOCKEDMEM_SIZE"
    echo "* hard memlock $MAXLOCKEDMEM_SIZE"
    sed -i '/hard memlock/d' /etc/security/limits.conf
    sed -i '/soft memlock/d' /etc/security/limits.conf
    if [[ -z "$(grep 'soft memlock' /etc/security/limits.conf)" ]]; then
      echo "* soft memlock $MAXLOCKEDMEM_SIZE" >> /etc/security/limits.conf
      echo "* hard memlock $MAXLOCKEDMEM_SIZE" >> /etc/security/limits.conf
      echo
    else
      sed -i "s|memlock .*|memlock $MAXLOCKEDMEM_SIZE|g" /etc/security/limits.conf
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
    elif [[ "$CENTOS_SEVEN" = '7' && "$HP_CHECK" = '[always]' ]]; then
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
    elif [[ "$CENTOS_SEVEN" = '7' && "$HP_CHECK" = '[never]' && "$PHP_HUGEPAGES" = [yY] && ! -f /proc/user_beancounters && "$FREEMEM" -ge '1980000' ]]; then
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