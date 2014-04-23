function funct_openvz_stacksize {

if [ -f /proc/user_beancounters ]; then

    echo ""
    echo "*************************************************"
    cecho "* Correct service's stack size for OpenVZ systems. Please wait...." $boldgreen
    echo "*************************************************"

sed -i 's/#!\/bin\/bash/#!\/bin\/bash\nif [ -f \/proc\/user_beancounters ]; then\nulimit -s 512\nfi\n/g' /etc/init.d/rsyslog

sed -i 's/#!\/bin\/bash/#!\/bin\/bash\nif [ -f \/proc\/user_beancounters ]; then\nulimit -s 256\nfi\n/g' /etc/init.d/nsd

sed -i 's/#!\/bin\/sh/#!\/bin\/sh\nif [ -f \/proc\/user_beancounters ]; then\nulimit -s 256\nfi\n/g' /etc/init.d/nginx

sed -i 's/#!\/bin\/sh/#!\/bin\/sh\nif [ -f \/proc\/user_beancounters ]; then\nulimit -s 2048\nfi\n/g' /etc/init.d/lfd

sed -i 's/#!\/bin\/sh/#!\/bin\/sh\nif [ -f \/proc\/user_beancounters ]; then\nulimit -s 512\nfi\n/g' /etc/init.d/mysql

echo "checking stack size ulimit -s set properly: "

head -n 5  /etc/init.d/rsyslog
head -n 5 /etc/init.d/nsd
head -n 5 /etc/init.d/nginx
head -n 5 /etc/init.d/lfd
head -n 5 /etc/init.d/mysql

RSYSLOGRUNNING=`service rsyslog status | awk '{ print $5 }' | sed 's/running.\.\./running/g'`

if [[ "$RSYSLOGRUNNING" = 'running' ]]; then
service rsyslog restart
fi

    echo "*************************************************"

fi

}