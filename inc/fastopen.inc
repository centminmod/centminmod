fastopenconfig() {
# tcp_fastopen autodetect
FASTOPEN_VAL=$(cat /proc/sys/net/ipv4/tcp_fastopen >/dev/null 2>&1; echo $?)
FASTOPEN_VAL=$(echo $FASTOPEN_VAL | awk '{print $NF}')
FASTOPEN_STATSVAL=$(grep '^TcpExt:' /proc/net/netstat | cut -d ' ' -f 87-92  | column -t | grep 'TCPFast'; echo $?)
FASTOPEN_STATSVAL=$(echo $FASTOPEN_STATSVAL | awk '{print $NF}')
CHECKREUSEPORT=$(grep --color -Ro SO_REUSEPORT /usr/src/kernels/* | head -n1 | awk -F ":" '{print $2}')

if [[ "$CHECKREUSEPORT" = 'SO_REUSEPORT' ]]; then
  SUPPORT_REUSEPORT=y
else
  SUPPORT_REUSEPORT=n
fi

if [[ -f /proc/sys/net/ipv4/tcp_fastopen && "$FASTOPEN_STATSVAL" = '0' ]]; then
  TCPFASTOPEN=1
  NGX_FOPENOPT=' -DTCP_FASTOPEN=23'
  if [ "$CENTOS_SIX" = '6' ]; then
    FOCHECK=$(grep 'net.ipv4.tcp_fastopen' /etc/sysctl.conf; echo $?)
    if [[ "$(echo $FOCHECK | awk '{print $NF}')" != '0' ]]; then
      echo "net.ipv4.tcp_fastopen=3" >> /etc/sysctl.conf
      sysctl -p
    fi
  elif [ "$CENTOS_SEVEN" = '7' ]; then
    echo 'net.ipv4.tcp_fastopen=3' > /etc/sysctl.d/50-tcp_fastopen.conf
    sysctl -p /etc/sysctl.d/50-tcp_fastopen.conf
  fi
  grep '^TcpExt:' /proc/net/netstat | cut -d ' ' -f 87-92  | column -t
else
  echo
  echo "system kernel does not support TCP FASTOPEN"
  echo
fi
if [[ "$TCPFASTOPEN" -eq '1' && "$SUPPORT_REUSEPORT" = [yY] ]]; then
  if [ -f /usr/local/nginx/conf/conf.d/virtual.conf ]; then
    if [ "$(grep -w 'listen 80 default_server backlog=2048 reuseport' /usr/local/nginx/conf/conf.d/virtual.conf)" ]; then
      sed -i 's|listen 80 default_server backlog=2048 reuseport.*|listen 80 default_server backlog=2048 reuseport fastopen=256;|' /usr/local/nginx/conf/conf.d/virtual.conf
    elif [ "$(grep -w 'listen   80 default_server backlog=2048 reuseport' /usr/local/nginx/conf/conf.d/virtual.conf)" ]; then
      sed -i 's|listen   80 default_server backlog=2048 reuseport.*|listen   80 default_server backlog=2048 reuseport fastopen=256;|' /usr/local/nginx/conf/conf.d/virtual.conf
    fi
  fi
elif [[ "$TCPFASTOPEN" -eq '1' && "$SUPPORT_REUSEPORT" = [nN] ]]; then
  if [ -f /usr/local/nginx/conf/conf.d/virtual.conf ]; then
    if [ "$(grep -w 'listen 80 default_server backlog=2048' /usr/local/nginx/conf/conf.d/virtual.conf)" ]; then
      sed -i 's|listen 80 default_server backlog=2048.*|listen 80 default_server backlog=2048 fastopen=256;|' /usr/local/nginx/conf/conf.d/virtual.conf
    elif [ "$(grep -w 'listen   80 default_server backlog=2048' /usr/local/nginx/conf/conf.d/virtual.conf)" ]; then
      sed -i 's|listen   80 default_server backlog=2048.*|listen   80 default_server backlog=2048 fastopen=256;|' /usr/local/nginx/conf/conf.d/virtual.conf
    fi
  fi
fi
}