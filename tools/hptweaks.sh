#!/bin/bash
#############################################################
# tweaks for centminmod.com LEMP stacks on CentOS 6/7
# centos transparent huge pages calculations for allocation
# of nr.hugepages and max locked memory system limits
# https://www.kernel.org/doc/Documentation/vm/hugetlbpage.txt
#############################################################
# set locale temporarily to english
# due to some non-english locale issues
export LC_ALL=en_US.UTF-8
export LANG=en_US.UTF-8
export LANGUAGE=en_US.UTF-8
export LC_CTYPE=en_US.UTF-8
# disable systemd pager so it doesn't pipe systemctl output to less
export SYSTEMD_PAGER=''
ARCH_CHECK="$(uname -m)"

shopt -s expand_aliases
for g in "" e f; do
    alias ${g}grep="LC_ALL=C ${g}grep"  # speed-up grep, egrep, fgrep
done

# Add LXC detection
if [[ ! -f /proc/user_beancounters ]]; then
    if [[ -f /usr/bin/systemd-detect-virt && "$(/usr/bin/systemd-detect-virt)" = 'lxc' ]]; then
        CHECK_LXD='y'
    elif [[ -f $(which virt-what) ]]; then
        VIRT_WHAT_OUTPUT=$(virt-what | xargs)
        if [[ $VIRT_WHAT_OUTPUT == *'openvz'* ]]; then
            CHECK_LXD='n'
        elif [[ $VIRT_WHAT_OUTPUT == *'lxc'* ]]; then
            CHECK_LXD='y'
        fi
    fi
fi

CENTOSVER=$(awk '{ print $3 }' /etc/redhat-release)

if [ "$CENTOSVER" == 'release' ]; then
    CENTOSVER=$(awk '{ print $4 }' /etc/redhat-release | cut -d . -f1,2)
    if [[ "$(cat /etc/redhat-release | awk '{ print $4 }' | cut -d . -f1)" = '7' ]]; then
        CENTOS_SEVEN='7'
    elif [[ "$(cat /etc/redhat-release | awk '{ print $4 }' | cut -d . -f1)" = '8' ]]; then
        CENTOS_EIGHT='8'
    elif [[ "$(cat /etc/redhat-release | awk '{ print $4 }' | cut -d . -f1)" = '9' ]]; then
        CENTOS_NINE='9'
    elif [[ "$(cat /etc/redhat-release | awk '{ print $4 }' | cut -d . -f1)" = '10' ]]; then
        CENTOS_TEN='10'
    fi
fi

if [[ "$(cat /etc/redhat-release | awk '{ print $3 }' | cut -d . -f1)" = '6' ]]; then
    CENTOS_SIX='6'
fi

  # check if redis installed as redis server requires huge pages disabled
  if [[ -f /usr/bin/redis-cli ]]; then
    if [[ -f /sys/kernel/mm/transparent_hugepage/enabled ]]; then
      echo madvise > /sys/kernel/mm/transparent_hugepage/enabled
      sed -i '/transparent_hugepage/d' /etc/rc.local
      if [[ -z "$(grep transparent_hugepage /etc/rc.local)" ]]; then
        echo "echo madvise > /sys/kernel/mm/transparent_hugepage/enabled" >> /etc/rc.local
      fi
      # extra workaround to ensure centos 7 systems boot redis server after rc.local
      # and that /sys/kernel/mm/transparent_hugepage/enabled is set to madvise as it seems
      # centos 7.4 at least restores value of always when rebooted 
      # https://community.centminmod.com/posts/57637/
      if [ -d /etc/systemd/system ]; then
        if [ -d /etc/systemd/system/redis.service.d ]; then
          # echo -e "[Unit]\nAfter=network.target rc.local" > /etc/systemd/system/redis.service.d/after-rc-local.conf
          systemctl daemon-reload
          systemctl restart redis
        fi
cat > "/etc/systemd/system/disable-thp.service" <<EOF
[Unit]
Description=Disable Transparent Huge Pages (THP)
After=network.target

[Service]
Type=simple
ExecStart=/bin/sh -c "/usr/bin/echo 'madvise' > /sys/kernel/mm/transparent_hugepage/enabled"

[Install]
WantedBy=multi-user.target
EOF

        if [ -f /etc/systemd/system/disable-thp.service ]; then
          chmod a+x /etc/systemd/system/disable-thp.service
          systemctl daemon-reload
          systemctl restart disable-thp
          systemctl enable disable-thp
          # echo
          # echo "cat /sys/kernel/mm/transparent_hugepage/enabled"
          # cat /sys/kernel/mm/transparent_hugepage/enabled
          # echo
          # echo "transparent_hugepage disabled"
          # echo
        fi
      fi
    fi
  else
    if [[ -f /sys/kernel/mm/transparent_hugepage/enabled ]]; then
      echo always > /sys/kernel/mm/transparent_hugepage/enabled
      sed -i '/transparent_hugepage/d' /etc/rc.local
      if [[ -z "$(grep transparent_hugepage /etc/rc.local)" ]]; then
        echo "echo always > /sys/kernel/mm/transparent_hugepage/enabled" >> /etc/rc.local
      fi
    fi
  fi

if [[ -f /sys/kernel/mm/transparent_hugepage/enabled ]]; then
  HP_CHECK=$(cat /sys/kernel/mm/transparent_hugepage/enabled | grep -o '\[.*\]')
fi

if [[ -f /sys/kernel/mm/transparent_hugepage/enabled ]]; then
  if [[ "$CENTOS_SIX" = '6' && "$HP_CHECK" = '[always]' ]]; then
    if [[ ! -f /proc/user_beancounters && -f /usr/bin/numactl ]]; then
      # account for multiple cpu socket numa based memory
      # https://community.centminmod.com/posts/48189/
      GETCPUNODE_COUNT=$(numactl --hardware | awk '/available: / {print $2}')
      if [[ "$GETCPUNODE_COUNT" -ge '2' ]]; then
        FREEMEM_NUMANODE=$(($(numactl --hardware | awk '/free:/ {print $4}' | sort -r | head -n1)*1024))
        FREEMEMCACHED=$(grep -E '^Buffers|^Cached' /proc/meminfo | awk '{summ+=$2} END {print summ}' | head -n1)
        FREEMEM=$(($FREEMEM_NUMANODE+$FREEMEMCACHED))
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
    NRHUGEPAGES_COUNT=$(($FREEMEM/2/2048/16*16/4))
    MAXLOCKEDMEM_COUNT=$(($FREEMEM/2/2048/16*16*4))
    MAXLOCKEDMEM_SIZE=$(($MAXLOCKEDMEM_COUNT*1024))
    MAXLOCKEDMEM_SIZE_NGINX=$(($MAXLOCKEDMEM_SIZE*32))
  elif [[ "$CENTOS_SEVEN" = '7' && "$HP_CHECK" = '[always]' ]]; then
    if [[ ! -f /proc/user_beancounters && -f /usr/bin/numactl ]]; then
      # account for multiple cpu socket numa based memory
      # https://community.centminmod.com/posts/48189/
      GETCPUNODE_COUNT=$(numactl --hardware | awk '/available: / {print $2}')
      if [[ "$GETCPUNODE_COUNT" -ge '2' ]]; then
        FREEMEM_NUMANODE=$(($(numactl --hardware | awk '/free:/ {print $4}' | sort -r | head -n1)*1024))
        FREEMEMCACHED=$(grep -E '^Buffers|^Cached' /proc/meminfo | awk '{summ+=$2} END {print summ}' | head -n1)
        FREEMEM=$(($FREEMEM_NUMANODE+$FREEMEMCACHED))
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
    NRHUGEPAGES_COUNT=$(($FREEMEM/2/2048/16*16/4))
    MAXLOCKEDMEM_COUNT=$(($FREEMEM/2/2048/16*16*4))
    MAXLOCKEDMEM_SIZE=$(($MAXLOCKEDMEM_COUNT*1024))
    MAXLOCKEDMEM_SIZE_NGINX=$(($MAXLOCKEDMEM_SIZE*32))
  elif [[ "$CENTOS_SEVEN" = '7' && "$HP_CHECK" = '[never]' ]]; then
    if [[ ! -f /proc/user_beancounters && -f /usr/bin/numactl ]]; then
      # account for multiple cpu socket numa based memory
      # https://community.centminmod.com/posts/48189/
      GETCPUNODE_COUNT=$(numactl --hardware | awk '/available: / {print $2}')
      if [[ "$GETCPUNODE_COUNT" -ge '2' ]]; then
        FREEMEM_NUMANODE=$(($(numactl --hardware | awk '/free:/ {print $4}' | sort -r | head -n1)*1024))
        FREEMEMCACHED=$(grep -E '^Buffers|^Cached' /proc/meminfo | awk '{summ+=$2} END {print summ}' | head -n1)
        FREEMEM=$(($FREEMEM_NUMANODE+$FREEMEMCACHED))
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
    NRHUGEPAGES_COUNT=$(($FREEMEM/2/2048/16*16/4))
    MAXLOCKEDMEM_COUNT=$(($FREEMEM/2/2048/16*16*4))
    MAXLOCKEDMEM_SIZE=$(($MAXLOCKEDMEM_COUNT*1024))
    MAXLOCKEDMEM_SIZE_NGINX=$(($MAXLOCKEDMEM_SIZE*32))
  fi
  
  if [[ "$NRHUGEPAGES_COUNT" -ge '1' ]]; then
    if [[ "$HP_CHECK" = '[always]' ]]; then
      echo
      echo "set vm.nr.hugepages in /etc/sysctl.conf"
      if [[ -z "$(grep '^vm.nr_hugepages' /etc/sysctl.conf)" ]]; then
        echo "vm.nr_hugepages=$NRHUGEPAGES_COUNT" >> /etc/sysctl.conf
        sysctl -p
      else
        sed -i "s|vm.nr_hugepages=.*|vm.nr_hugepages=$NRHUGEPAGES_COUNT|" /etc/sysctl.conf
        sysctl -p
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
        cat /etc/security/limits.conf
        echo
        ulimit -H -l
      fi
    elif [[ "$HP_CHECK" = '[never]' ]]; then
      echo
      echo "set vm.nr.hugepages in /etc/sysctl.conf"
      if [[ -z "$(grep '^vm.nr_hugepages' /etc/sysctl.conf)" ]]; then
        echo "vm.nr_hugepages=$NRHUGEPAGES_COUNT" >> /etc/sysctl.conf
        sysctl -p
      else
        sed -i "s|vm.nr_hugepages=.*|vm.nr_hugepages=$NRHUGEPAGES_COUNT|" /etc/sysctl.conf
        sysctl -p
      fi
    fi
  else
    echo "NRHUGEPAGES_COUNT = $NRHUGEPAGES_COUNT"
    echo "transparent huge pages not enabled"
    if [[ -f /sys/kernel/mm/transparent_hugepage/enabled ]]; then
      echo madvise > /sys/kernel/mm/transparent_hugepage/enabled
      if [[ -z "$(grep transparent_hugepage /etc/rc.local)" ]]; then
        echo "echo madvise > /sys/kernel/mm/transparent_hugepage/enabled" >> /etc/rc.local
      fi
    fi
  fi # end NRHUGEPAGES_COUNT > 0 check
elif [[ "$HP_CHECK" = '[never]' ]]; then
  echo
  echo "transparent huge pages not enabled"
  echo "no tweaks needed"
  echo  
else
  echo
  echo "transparent huge pages not supported"
  echo "no tweaks needed"
  echo
fi