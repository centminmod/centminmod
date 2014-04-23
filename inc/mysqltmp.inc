mysqltmpdir() {

if [ -d "/home/mysqltmp" ]; then
    if [[ $(stat -c %U /home/mysqltmp/) != 'mysql' && -f /usr/bin/mysql ]]; then 
        chown mysql:mysql /home/mysqltmp/
    fi
fi

}