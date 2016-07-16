opt_tcp() {
#######################################################
# check if custom open file descriptor limits already exist
    LIMITSCONFCHECK=`grep '* hard nofile 262144' /etc/security/limits.conf`
    if [[ -z $LIMITSCONFCHECK ]]; then
        # Set VPS hard/soft limits
        echo "* soft nofile 262144" >>/etc/security/limits.conf
        echo "* hard nofile 262144" >>/etc/security/limits.conf
        ulimit -n 262144
        echo "ulimit -n 262144" >> /etc/rc.local
    fi # check if custom open file descriptor limits already exist

    if [[ "$CENTOS_SEVEN" = '7' ]]; then
        # centos 7
        if [[ -f /etc/security/limits.d/20-nproc.conf ]]; then
cat > "/etc/security/limits.d/20-nproc.conf" <<EOF
# Default limit for number of user's processes to prevent
# accidental fork bombs.
# See rhbz #432903 for reasoning.

*          soft    nproc     8192
*          hard    nproc     8192
nginx      soft    nproc     32278
nginx      hard    nproc     32278
root       soft    nproc     unlimited
EOF
      fi
    else
        # centos 6
        if [[ -f /etc/security/limits.d/90-nproc.conf ]]; then
cat > "/etc/security/limits.d/90-nproc.conf" <<EOF
# Default limit for number of user's processes to prevent
# accidental fork bombs.
# See rhbz #432903 for reasoning.

*          soft    nproc     8192
*          hard    nproc     8192
nginx      soft    nproc     32278
nginx      hard    nproc     32278
root       soft    nproc     unlimited
EOF
        fi # raise user process limits
    fi

  # lnstat -c -1 -f nf_conntrack
  TCPMEMTOTAL=$(awk '/MemTotal/ {print $2}' /proc/meminfo)
  # mem usage ~ NF_CTMAX x 288 bytes
  # mem usage ~ NF_CTHASHSIZE x 8 bytes
  if [ "$TCPMEMTOTAL" -le '770000' ]; then
    NF_CTMAX='131072'
    NF_CTHASHSIZE=$(($(awk -F ": " '/cache size/ {print $2}' /proc/cpuinfo | sed -e 's| KB||' | uniq) *1024*6/10/8))
  elif [[ "$TCPMEMTOTAL" -gt '770000' && "$TCPMEMTOTAL" -le '1049000' ]]; then
    NF_CTMAX='262144'
    NF_CTHASHSIZE=$(($(awk -F ": " '/cache size/ {print $2}' /proc/cpuinfo | sed -e 's| KB||' | uniq) *1024*6/10/8))
  elif [[ "$TCPMEMTOTAL" -gt '1049000' && "$TCPMEMTOTAL" -le '2098000' ]]; then
    NF_CTMAX='524288'
    NF_CTHASHSIZE=$(($(awk -F ": " '/cache size/ {print $2}' /proc/cpuinfo | sed -e 's| KB||' | uniq) *1024*6/10/8))
  elif [[ "$TCPMEMTOTAL" -gt '2098000' && "$TCPMEMTOTAL" -le '3147000' ]]; then
    NF_CTMAX='524288'
    NF_CTHASHSIZE=$(($(awk -F ": " '/cache size/ {print $2}' /proc/cpuinfo | sed -e 's| KB||' | uniq) *1024*6/10/8))
  elif [[ "$TCPMEMTOTAL" -gt '3147000' && "$TCPMEMTOTAL" -le '4196000' ]]; then
    NF_CTMAX='524288'
    NF_CTHASHSIZE=$(($(awk -F ": " '/cache size/ {print $2}' /proc/cpuinfo | sed -e 's| KB||' | uniq) *1024*6/10/8))
  elif [[ "$TCPMEMTOTAL" -gt '4196000' && "$TCPMEMTOTAL" -le '8392000' ]]; then
    NF_CTMAX='524288'
    NF_CTHASHSIZE=$(($(awk -F ": " '/cache size/ {print $2}' /proc/cpuinfo | sed -e 's| KB||' | uniq) *1024*6/10/8))
  elif [[ "$TCPMEMTOTAL" -gt '8392000' && "$TCPMEMTOTAL" -le '16784000' ]]; then
    NF_CTMAX='1048576'
    NF_CTHASHSIZE=$(($(awk -F ": " '/cache size/ {print $2}' /proc/cpuinfo | sed -e 's| KB||' | uniq) *1024*6/10/8))
  elif [[ "$TCPMEMTOTAL" -gt '16784000' && "$TCPMEMTOTAL" -le '33568000' ]]; then
    NF_CTMAX='1048576'
    NF_CTHASHSIZE=$(($(awk -F ": " '/cache size/ {print $2}' /proc/cpuinfo | sed -e 's| KB||' | uniq) *1024*6/10/8))
  elif [[ "$TCPMEMTOTAL" -gt '33568000' && "$TCPMEMTOTAL" -le '67136000' ]]; then
    NF_CTMAX='1048576'
    NF_CTHASHSIZE=$(($(awk -F ": " '/cache size/ {print $2}' /proc/cpuinfo | sed -e 's| KB||' | uniq) *1024*6/10/8))
  elif [[ "$TCPMEMTOTAL" -gt '67136000' && "$TCPMEMTOTAL" -le '134272000' ]]; then
    NF_CTMAX='1875008'
    NF_CTHASHSIZE=$(($(awk -F ": " '/cache size/ {print $2}' /proc/cpuinfo | sed -e 's| KB||' | uniq) *1024*6/10/8))
  elif [[ "$TCPMEMTOTAL" -gt '134272000' && "$TCPMEMTOTAL" -le '268544000' ]]; then
    NF_CTMAX='1875008'
    NF_CTHASHSIZE=$(($(awk -F ": " '/cache size/ {print $2}' /proc/cpuinfo | sed -e 's| KB||' | uniq) *1024*6/10/8))
  elif [ "$TCPMEMTOTAL" -gt '268544000' ]; then
    NF_CTMAX='1875008'
    NF_CTHASHSIZE=$(($(awk -F ": " '/cache size/ {print $2}' /proc/cpuinfo | sed -e 's| KB||' | uniq) *1024*6/10/8))
  fi

if [[ ! -f /proc/user_beancounters ]]; then
    if [[ "$CENTOS_SEVEN" = '7' ]]; then
        if [ -d /etc/sysctl.d ]; then
            # raise hashsize for conntrack entries
            echo "$NF_CTHASHSIZE" > /sys/module/nf_conntrack/parameters/hashsize
            if [[ "$(grep 'hashsize' /etc/rc.local >/dev/null 2>&1; echo $?)" != '0' ]]; then
                echo "echo $NF_CTHASHSIZE > /sys/module/nf_conntrack/parameters/hashsize" >> /etc/rc.local
            fi
            # centos 7
            touch /etc/sysctl.d/101-sysctl.conf
            if [[ "$(grep 'centminmod added' /etc/sysctl.d/101-sysctl.conf >/dev/null 2>&1; echo $?)" != '0' ]]; then
cat >> "/etc/sysctl.d/101-sysctl.conf" <<EOF
# centminmod added
fs.nr_open=12000000
fs.file-max=9000000
net.core.wmem_max=16777216
net.core.rmem_max=16777216
net.ipv4.tcp_rmem=8192 87380 16777216                                          
net.ipv4.tcp_wmem=8192 65536 16777216
net.core.netdev_max_backlog=8192
net.core.somaxconn=8151
net.core.optmem_max=8192
net.ipv4.tcp_fin_timeout=10
net.ipv4.tcp_keepalive_intvl=30
net.ipv4.tcp_keepalive_probes=3
net.ipv4.tcp_keepalive_time=240
net.ipv4.tcp_max_syn_backlog=8192
net.ipv4.tcp_sack=1
net.ipv4.tcp_syn_retries=3
net.ipv4.tcp_synack_retries = 2
net.ipv4.tcp_tw_recycle = 1
net.ipv4.tcp_tw_reuse = 1
net.ipv4.tcp_max_tw_buckets = 1440000
vm.swappiness=10
vm.min_free_kbytes=65536
net.ipv4.ip_local_port_range=1024 65535
net.ipv4.tcp_slow_start_after_idle=0
net.ipv4.tcp_limit_output_bytes=65536
net.ipv4.tcp_rfc1337=1
net.ipv4.conf.all.accept_redirects = 0
net.ipv4.conf.all.accept_source_route = 0
net.ipv4.conf.all.log_martians = 1
net.ipv4.conf.all.rp_filter = 1
net.ipv4.conf.all.secure_redirects = 0
net.ipv4.conf.all.send_redirects = 0
net.ipv4.conf.default.accept_redirects = 0
net.ipv4.conf.default.accept_source_route = 0
net.ipv4.conf.default.log_martians = 1
net.ipv4.conf.default.rp_filter = 1
net.ipv4.conf.default.secure_redirects = 0
net.ipv4.conf.default.send_redirects = 0
net.ipv4.icmp_echo_ignore_broadcasts = 1
net.ipv4.icmp_ignore_bogus_error_responses = 1
net.netfilter.nf_conntrack_helper=0
net.netfilter.nf_conntrack_max = $NF_CTMAX
net.netfilter.nf_conntrack_tcp_timeout_established = 28800
net.netfilter.nf_conntrack_generic_timeout = 60
EOF
        /sbin/sysctl --system
            fi           
        fi
    else
        # centos 6
        if [[ "$(grep 'centminmod added' /etc/sysctl.conf >/dev/null 2>&1; echo $?)" != '0' ]]; then
            if [ -f /sys/module/nf_conntrack/parameters/hashsize ]; then
              # raise hashsize for conntrack entries
              echo "$NF_CTHASHSIZE" > /sys/module/nf_conntrack/parameters/hashsize
              if [[ "$(grep 'hashsize' /etc/rc.local >/dev/null 2>&1; echo $?)" != '0' ]]; then
                  echo "echo $NF_CTHASHSIZE > /sys/module/nf_conntrack/parameters/hashsize" >> /etc/rc.local
              fi
            fi
cat >> "/etc/sysctl.conf" <<EOF
# centminmod added
fs.nr_open=12000000
fs.file-max=9000000
net.core.wmem_max=16777216
net.core.rmem_max=16777216
net.ipv4.tcp_rmem=8192 87380 16777216                                          
net.ipv4.tcp_wmem=8192 65536 16777216
net.core.netdev_max_backlog=8192
net.core.somaxconn=8151
net.core.optmem_max=8192
net.ipv4.tcp_fin_timeout=10
net.ipv4.tcp_keepalive_intvl=30
net.ipv4.tcp_keepalive_probes=3
net.ipv4.tcp_keepalive_time=240
net.ipv4.tcp_max_syn_backlog=8192
net.ipv4.tcp_sack=1
net.ipv4.tcp_syn_retries=3
net.ipv4.tcp_synack_retries = 2
net.ipv4.tcp_tw_recycle = 1
net.ipv4.tcp_tw_reuse = 1
net.ipv4.tcp_max_tw_buckets = 1440000
vm.swappiness=10
vm.min_free_kbytes=65536
net.ipv4.ip_local_port_range=1024 65535
net.ipv4.tcp_slow_start_after_idle=0
net.ipv4.tcp_rfc1337=1
net.ipv4.conf.all.accept_redirects = 0
net.ipv4.conf.all.accept_source_route = 0
net.ipv4.conf.all.log_martians = 1
net.ipv4.conf.all.rp_filter = 1
net.ipv4.conf.all.secure_redirects = 0
net.ipv4.conf.all.send_redirects = 0
net.ipv4.conf.default.accept_redirects = 0
net.ipv4.conf.default.accept_source_route = 0
net.ipv4.conf.default.log_martians = 1
net.ipv4.conf.default.rp_filter = 1
net.ipv4.conf.default.secure_redirects = 0
net.ipv4.conf.default.send_redirects = 0
net.ipv4.icmp_echo_ignore_broadcasts = 1
net.ipv4.icmp_ignore_bogus_error_responses = 1
net.netfilter.nf_conntrack_helper=0
net.netfilter.nf_conntrack_max = $NF_CTMAX
net.netfilter.nf_conntrack_tcp_timeout_established = 28800
net.netfilter.nf_conntrack_generic_timeout = 60
EOF
sysctl -p
        fi
    fi # centos 6 or 7
fi
}