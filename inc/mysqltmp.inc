mysqltmpdir() {

    if [ -f "$(which figlet)" ]; then
        figlet -ckf standard "mysql tmpdir setup"
    fi

if [ -d "/home/mysqltmp" ]; then
    if [[ $(stat -c %U /home/mysqltmp/) != 'mysql' && -f /usr/bin/mysql ]]; then 
        chown mysql:mysql /home/mysqltmp/
    fi
fi

}