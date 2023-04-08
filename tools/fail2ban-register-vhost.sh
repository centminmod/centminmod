#!/bin/bash
####################################################################
# for Centmin Mod newly created Nginx vhosts to register their 
# logpaths in fail2ban, it requires fail2ban service restart
# intended for Centmin Mod's fail2ban implementation usage
# https://github.com/centminmod/centminmod-fail2ban/tree/1.0
####################################################################
# set locale temporarily to english
# due to some non-english locale issues
export LC_ALL=en_US.UTF-8
export LANG=en_US.UTF-8
export LANGUAGE=en_US.UTF-8
export LC_CTYPE=en_US.UTF-8
# disable systemd pager so it doesn't pipe systemctl output to less
export SYSTEMD_PAGER=''

# Check if fail2ban service is running
if systemctl is-active fail2ban >/dev/null 2>&1; then
  # Check if the nginx filter file exists
  if [ -f /etc/fail2ban/filter.d/nginx-common-main.conf ]; then
    # Restart fail2ban service silently
    systemctl restart fail2ban >/dev/null 2>&1
    echo
    echo "Fail2ban service restarted"
    echo "${1} vhost logpaths registered with Fail2ban"
    echo
  fi
fi
