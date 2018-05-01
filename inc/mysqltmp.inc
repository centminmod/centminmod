c7tmpsetup() {
  if [[ "$CENTOS_SEVEN" = '7' && ! -f /etc/systemd/system/mariadb.service.d/protecthome.conf && -d /etc/systemd/system/mariadb.service.d/ ]]; then
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