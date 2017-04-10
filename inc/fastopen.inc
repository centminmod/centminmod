fastopenconfig() {
# tcp_fastopen autodetect
FASTOPEN_VAL=$(cat /proc/sys/net/ipv4/tcp_fastopen; echo $?)
FASTOPEN_VAL=$(echo $FASTOPEN_VAL | awk '{print $NF}')
FASTOPEN_STATSVAL=$(grep '^TcpExt:' /proc/net/netstat | cut -d ' ' -f 87-92  | column -t | grep 'TCPFastOpenActive'; echo $?)
FASTOPEN_STATSVAL=$(echo $FASTOPEN_STATSVAL | awk '{print $NF}')

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
}