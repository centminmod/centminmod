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

shopt -s expand_aliases
for g in "" e f; do
    alias ${g}grep="LC_ALL=C ${g}grep"  # speed-up grep, egrep, fgrep
done

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
	else
		echo "NRHUGEPAGES_COUNT = $NRHUGEPAGES_COUNT"
		if [[ -f /sys/kernel/mm/transparent_hugepage/enabled ]]; then
			echo never > /sys/kernel/mm/transparent_hugepage/enabled
			if [[ -z "$(grep transparent_hugepage /etc/rc.local)" ]]; then
				echo "echo never > /sys/kernel/mm/transparent_hugepage/enabled" >> /etc/rc.local
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