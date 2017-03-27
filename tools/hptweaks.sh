#!/bin/bash
#############################################################
# tweaks for centminmod.com LEMP stacks on CentOS 6/7
# centos transparent huge pages calculations for allocation
# of nr.hugepages and max locked memory system limits
# https://www.kernel.org/doc/Documentation/vm/hugetlbpage.txt
#############################################################

CENTOSVER=$(awk '{ print $3 }' /etc/redhat-release)

if [ "$CENTOSVER" == 'release' ]; then
    CENTOSVER=$(awk '{ print $4 }' /etc/redhat-release | cut -d . -f1,2)
    if [[ "$(cat /etc/redhat-release | awk '{ print $4 }' | cut -d . -f1)" = '7' ]]; then
        CENTOS_SEVEN='7'
    fi
fi

if [[ "$(cat /etc/redhat-release | awk '{ print $3 }' | cut -d . -f1)" = '6' ]]; then
    CENTOS_SIX='6'
fi

	# check if redis installed as redis server requires huge pages disabled
	if [[ -f /usr/bin/redis-cli ]]; then
		if [[ -f /sys/kernel/mm/transparent_hugepage/enabled ]]; then
			echo never > /sys/kernel/mm/transparent_hugepage/enabled
			if [[ -z "$(grep transparent_hugepage /etc/rc.local)" ]]; then
				echo "echo never > /sys/kernel/mm/transparent_hugepage/enabled" >> /etc/rc.local
			fi
		fi
	fi

if [[ -f /sys/kernel/mm/transparent_hugepage/enabled ]]; then
	HP_CHECK=$(cat /sys/kernel/mm/transparent_hugepage/enabled | grep -o '\[.*\]')
fi

if [[ -f /sys/kernel/mm/transparent_hugepage/enabled ]]; then
	if [[ "$CENTOS_SIX" = '6' && "$HP_CHECK" = '[always]' ]]; then
		FREEMEM=$(cat /proc/meminfo | grep MemFree | awk '{print $2}')
		NRHUGEPAGES_COUNT=$(($FREEMEM/8/2048/16*16))
		MAXLOCKEDMEM_COUNT=$(($FREEMEM/8/2048/16*16*4))
		MAXLOCKEDMEM_SIZE=$((MAXLOCKEDMEM_COUNT*1024))
	elif [[ "$CENTOS_SEVEN" = '7' && "$HP_CHECK" = '[always]' ]]; then
		FREEMEM=$(cat /proc/meminfo | grep MemAvailable | awk '{print $2}')
		NRHUGEPAGES_COUNT=$(($FREEMEM/8/2048/16*16))
		MAXLOCKEDMEM_COUNT=$(($FREEMEM/8/2048/16*16*4))
		MAXLOCKEDMEM_SIZE=$((MAXLOCKEDMEM_COUNT*1024))
	fi
	
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
		echo
		echo "set system max locked memory limit"
		echo
		echo "/etc/security/limits.conf"
		echo "* soft memlock $MAXLOCKEDMEM_SIZE"
		echo "* hard memlock $MAXLOCKEDMEM_SIZE"
		if [[ -z "$(grep '^memlock' /etc/security/limits.conf)" ]]; then
			echo "* soft memlock $MAXLOCKEDMEM_SIZE" >> /etc/security/limits.conf
			echo "* hard memlock $MAXLOCKEDMEM_SIZE" >> /etc/security/limits.conf
			echo
		else
			sed -i "s|memlock .*|memlock $MAXLOCKEDMEM_SIZE|g" /etc/security/limits.conf
		fi
		cat /etc/security/limits.conf
		echo
	fi
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