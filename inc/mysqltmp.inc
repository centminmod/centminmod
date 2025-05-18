c7tmpsetup() {
  if [[ "$CENTOS_SEVEN" -eq '7' || "$CENTOS_EIGHT" -eq '8' || "$CENTOS_NINE" -eq '9' || "$CENTOS_TEN" -eq '10' ]] && [[ ! -f /etc/systemd/system/mariadb.service.d/protecthome.conf && -d /etc/systemd/system/mariadb.service.d/ ]]; then
    if [ ! -d /etc/systemd/system/mariadb.service.d/ ]; then
      mkdir -p /etc/systemd/system/mariadb.service.d/
    fi
    echo "Update MariaDB 10 setting for ProtectHome=false"
cat > "/etc/systemd/system/mariadb.service.d/protecthome.conf" <<TDF
[Service]
ProtectSystem=false
ProtectHome=false
TDF
    systemctl daemon-reload
  fi
}

mysqltmpdir() {

    if [[ "$INITIALINSTALL" = [yY] ]]; then
    if [ -f "$(which figlet)" ]; then
        figlet -ckf standard "mysql tmpdir setup"
    fi
    fi

if [ -d "/home/mysqltmp" ]; then
    if [[ $(stat -c %U /home/mysqltmp/) != 'mysql' && -f /usr/bin/mysql ]]; then 
        chown mysql:mysql /home/mysqltmp/
    fi
fi

c7tmpsetup
}