#!/bin/bash
VER=0.5
DT=$(date +"%d%m%y-%H%M%S")
SSH_LOGGING='n'
remote_port=22; remote_user="root"; remote_server=""
netcat="y"; buffer_size=131072; listen_port=12345
backupdir=""; remote_backupdir=""; private_key=""; comp_level=8

print_usage() {
  cat <<- EOF
Usage: $0 [options]

Options:
  -p   remote_port       Remote server SSH port (default: 22)
  -u   remote_user       Remote server SSH username (default: root)
  -h   remote_server     Remote server SSH hostname/IP address
  -m   tunnel_method     Tunnel method: 'nc' or 'socat' (default: nc)
  -b   buffer_size       Buffer size for socat (in bytes, e.g., 262144 for 256 KB)
  -l  listen_port       nc or socat listen port (default: 12345)
  -s source_directory Source backup directory
  -r remote_directory Remote (destination) backup directory
  -k   private_key       Path to the SSH private key

Example:
  $0 -p 2222 -u remoteuser -h 192.168.1.100 -m socat -b 262144 -l 23456 -s /source/backupdir -r /destination/backupdir -k ~/.ssh/id_rsa
EOF
}

check_remote_port_available() {
  local remote_host="$1"
  local port="$2"
  local result

  result=$($ssh_cmd "${remote_user}@${remote_host}" "sudo lsof -i :${port}" 2>/dev/null)

  if [ -z "$result" ]; then
    return 0
  else
    return 1
  fi
}

if [ "$#" -eq 0 ]; then
  print_usage
  exit 1
fi

# List of required commands
commands=("tar" "zstd" "pigz" "socat" "nc" "pv" "sshpass")

# Packages corresponding to the required commands
packages=("tar" "zstd" "pigz" "socat" "nmap-ncat" "pv" "sshpass")

# Check if the system is CentOS 7 or AlmaLinux 8
os_version=$(rpm -E %{rhel})

if [ "$os_version" -eq 7 ] || [ "$os_version" -eq 8 ] || [ "$os_version" -eq 9 ]; then
  # Check each command and install the corresponding package if the command is not found
  for i in "${!commands[@]}"; do
    command -v "${commands[$i]}" >/dev/null 2>&1 || {
      echo "Installing package for ${commands[$i]}..."
      sudo yum install -y "${packages[$i]}"
    }
  done
else
  echo "This script only supports CentOS 7 and AlmaLinux 8."
  echo
  exit 1
fi

# Parse arguments
while getopts "p:u:h:m:b:l:s:r:k:" opt; do
  case $opt in
    p) remote_port="$OPTARG" ;;
    u) remote_user="$OPTARG" ;;
    h) remote_server="$OPTARG" ;;
    m) [ "$OPTARG" == "nc" ] && netcat="y" || netcat="n" ;;
    b) buffer_size="$OPTARG" ;;
    l) listen_port="$OPTARG" ;;
    s) backupdir="$OPTARG"; [ ! -d "$backupdir" ] && echo "Error: Source directory does not exist" && exit 1 ;;
    r) remote_backupdir="$OPTARG" ;;
    k) private_key="$OPTARG" ;;
    *) print_usage; exit 1   ;;
  esac
done

# Add error handling checks here
if [ -z "$remote_server" ]; then
  echo "Error: Remote server address is required."
  exit 1
fi

if ! ping -c 1 -W 1 "$remote_server" > /dev/null 2>&1; then
  echo "Error: Remote server address is not valid or unreachable."
  exit 1
fi

if [ -n "$private_key" ]; then
  if [ ! -f "$private_key" ]; then
    echo "Error: SSH private key file not found."
    exit 1
  fi
else
  echo "Warning: No SSH private key provided. Default SSH key will be used."
fi

if ! [[ "$buffer_size" =~ ^[1-9][0-9]*$ ]]; then
  echo "Error: Invalid buffer size. Buffer size must be a positive integer."
  exit 1
fi

if ! [[ "$listen_port" =~ ^[1-9][0-9]*$ ]] || [ "$listen_port" -lt 1 ] || [ "$listen_port" -gt 65535 ]; then
  echo "Error: Invalid listen port. Listen port must be between 1 and 65535."
  exit 1
fi

if [[ "$SSH_LOGGING" = [yY] ]]; then
  ssh_cmd="ssh -v -o Ciphers=aes128-gcm@openssh.com,aes256-gcm@openssh.com,aes128-ctr,aes192-ctr,aes256-ctr,aes128-cbc,aes192-cbc,aes256-cbc -p $remote_port"
  ssh_debug_log="2>ssh_debug.log"
else
  ssh_cmd="ssh -o Ciphers=aes128-gcm@openssh.com,aes256-gcm@openssh.com,aes128-ctr,aes192-ctr,aes256-ctr,aes128-cbc,aes192-cbc,aes256-cbc -p $remote_port"
  ssh_debug_log=""
fi
[ -n "$private_key" ] && ssh_cmd="$ssh_cmd -i $private_key"

# if [[ "$remote_user" && "$remote_server" ]]; then
#   remote_dir_check=$($ssh_cmd "$remote_user@$remote_server" 'if [ -d "'"$remote_backupdir"'" ]; then echo "1"; else echo "0"; fi')
#   if [ "$remote_dir_check" == "0" ]; then
#     echo "Error: Remote directory does not exist"
#     exit 1
#   fi
# fi

if [[ "$remote_user" && "$remote_server" ]]; then
  # Check if the remote listen port is available
  check_remote_port_available "${remote_server}" "${listen_port}"
  port_check_result=$?
  if [ "$port_check_result" -eq 1 ]; then
    echo "Error: The remote listen port ${listen_port} is already in use."
    exit 1
  fi
  # Start netcat or socat listener on remote server
  if [ "${netcat}" == "y" ]; then
    listener_cmd="mkdir -p ${remote_backupdir} && nc -l ${listen_port} | zstd -d | tar -xf - -C ${remote_backupdir}"
  else
    listener_cmd="mkdir -p ${remote_backupdir} && socat -u TCP-LISTEN:${listen_port},rcvbuf=${buffer_size} - | zstd -d | tar -xf - -C ${remote_backupdir}"
  fi
  $ssh_cmd "${remote_user}@${remote_server}" "$listener_cmd" $ssh_debug_log &
  # Give the remote server a few seconds to start the listener
  sleep 5
  # Save the start time
  start_time=$(date +%s)

  # Execute the transfer commands and save the exit status
  if [ "${netcat}" == "y" ]; then
    (cd "${backupdir}" && tar -cpf - .) | pv -s $(du -sb "${backupdir}" | awk '{print $1}') | zstd -T$(nproc) --fast="${comp_level}" | nc "${remote_server}" "${listen_port}"
    transfer_status=$?
  else
    (cd "${backupdir}" && tar -cpf - .) | pv -s $(du -sb "${backupdir}" | awk '{print $1}') | zstd -T$(nproc) --fast="${comp_level}" | socat -u - TCP:"${remote_server}:${listen_port}",sndbuf=${buffer_size}
    transfer_status=$?
  fi

  # Save the end time
  end_time=$(date +%s)

  # Calculate the elapsed time and print the transfer status
  elapsed_time=$((end_time - start_time))
  if [ "$elapsed_time" -eq 0 ]; then
    time_display=""
  else
    time_display=" in $elapsed_time seconds."
  fi
  if [ "$transfer_status" -eq 0 ]; then
    echo "Transfer completed successfully${time_display}"
  else
    echo "Transfer failed with exit status $transfer_status${time_display}"
  fi
  # Wait for the background ssh process to complete
  wait
fi