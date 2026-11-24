fastopenconfig() {
# tcp_fastopen autodetect
FASTOPEN_VAL=$(cat /proc/sys/net/ipv4/tcp_fastopen >/dev/null 2>&1; echo $?)
FASTOPEN_VAL=$(echo $FASTOPEN_VAL | awk '{print $NF}')
FASTOPEN_STATSVAL=$(grep '^TcpExt:' /proc/net/netstat | cut -d ' ' -f 87-92  | column -t | grep 'TCPFast'; echo $?)
FASTOPEN_STATSVAL=$(echo $FASTOPEN_STATSVAL | awk '{print $NF}')
if [[ -f /proc/sys/net/ipv4/tcp_fastopen && "$FASTOPEN_STATSVAL" = '0' ]]; then
  TCPFASTOPEN=1
  if [[ "$TCP_FASTOPEN_ENABLE" = [yY] ]]; then
    NGX_FOPENOPT=' -DTCP_FASTOPEN=23'
  else
    NGX_FOPENOPT=""
  fi
  if [ "$CENTOS_SIX" = '6' ]; then
    FOCHECK=$(grep 'net.ipv4.tcp_fastopen' /etc/sysctl.conf; echo $?)
    if [[ "$(echo $FOCHECK | awk '{print $NF}')" != '0' ]]; then
      echo "net.ipv4.tcp_fastopen=3" >> /etc/sysctl.conf
      sysctl -p
    fi
  elif [[ "$TCP_FASTOPEN_ENABLE" = [yY] ]] && [[ "$CENTOS_SEVEN" -eq '7' || "$CENTOS_EIGHT" -eq '8' || "$CENTOS_NINE" -eq '9' || "$CENTOS_TEN" -eq '10' ]]; then
    echo 'net.ipv4.tcp_fastopen=3' > /etc/sysctl.d/50-tcp_fastopen.conf
    sysctl -p /etc/sysctl.d/50-tcp_fastopen.conf
  elif [[ "$TCP_FASTOPEN_ENABLE" != [yY] ]] && [[ "$CENTOS_SEVEN" -eq '7' || "$CENTOS_EIGHT" -eq '8' || "$CENTOS_NINE" -eq '9' || "$CENTOS_TEN" -eq '10' ]]; then
    if [ -f /etc/sysctl.d/50-tcp_fastopen.conf ]; then
      rm -f /etc/sysctl.d/50-tcp_fastopen.conf
      # removing the file does not change the running value: restore the kernel
      # default (1 = client side only), then re-apply the other sysctl settings
      sysctl -q -w net.ipv4.tcp_fastopen=1
      sysctl -q --system >/dev/null 2>&1
    fi
  fi
  grep '^TcpExt:' /proc/net/netstat | cut -d ' ' -f 87-92  | column -t
else
  echo
  echo "system kernel does not support TCP FASTOPEN"
  echo
fi
if [[ "$TCP_FASTOPEN_ENABLE" = [yY] ]] && [[ "$TCPFASTOPEN" -eq '1' ]]; then
  if [ -f /usr/local/nginx/conf/conf.d/virtual.conf ]; then
    # add fastopen=256 to the default listen 80 line whatever its backlog value and
    # other options (a literal backlog=2048 match missed every autotuned line and
    # rewrote the rest of the line); reuseport is left to its own checks
    # only the listen directive itself counts: text after its first ';' or after a '#'
    # (a trailing comment or a second statement) is never matched or edited
    sed -i -E '/^[[:space:]]*listen[[:space:]]+80[[:space:]]+default_server[^;#]*fastopen=/!s/^([[:space:]]*listen[[:space:]]+80[[:space:]]+default_server([^;#]*[^[:space:];#])?)[[:space:]]*;/\1 fastopen=256;/' /usr/local/nginx/conf/conf.d/virtual.conf
  fi
elif [[ "$TCP_FASTOPEN_ENABLE" != [yY] ]]; then
  if [ -f /usr/local/nginx/conf/conf.d/virtual.conf ]; then
    # remove only the exact fastopen=256 token; a longer value such as fastopen=2560
    # must not leave a bare number behind (nginx: invalid parameter)
    sed -i -E 's/ fastopen=256([^0-9]|$)/\1/g' /usr/local/nginx/conf/conf.d/virtual.conf
  fi
fi
}