# logrotate
# nano -w /etc/logrotate.d/nginx

function funct_listlogrotatedir {

echo "---------------------------------------------------------------------------"
echo "Current nginx log directories:"
echo "---------------------------------------------------------------------------"
if [ -d /var/log/nginx/ ]; then
echo "ls -lh /var/log/nginx/"
ls -lh /var/log/nginx/
echo "---------------------------------------------------------------------------"
fi

if [ -d /usr/local/nginx/logs/ ]; then
echo "ls -lh /usr/local/nginx/logs/"
ls -lh /usr/local/nginx/logs/
echo "---------------------------------------------------------------------------"
fi

ls -lh /home/nginx/domains/*/log/
echo "---------------------------------------------------------------------------"
echo ""

}

function funct_logrotate {

if [ ! -f /etc/logrotate.d/nginx ]; then

echo "----------------------------------------------"
echo "Creating /etc/logrotate.d/nginx file"
echo "----------------------------------------------"
echo "......"

touch /etc/logrotate.d/nginx

cat > "/etc/logrotate.d/nginx" <<END
/var/log/nginx/*.log /usr/local/nginx/logs/*.log /home/nginx/domains/*/log/*.log {
        daily
        missingok
        rotate 10
        size=500M
        compress
        delaycompress
        notifempty
        postrotate
        /bin/kill -SIGUSR1 `cat /usr/local/nginx/logs/nginx.pid 2>/dev/null` 2>/dev/null || true
        endscript            
}
END

echo "----------------------------------------------------------------------------"
cat /etc/logrotate.d/nginx

funct_listlogrotatedir

echo "----------------------------------------------------------------------------"
echo "nginx logrotation /etc/logrotate.d/nginx setup completed"
echo "----------------------------------------------------------------------------"

else 

echo "--------------------------------------------------"
echo " /etc/logrotate.d/nginx already exists"
echo "--------------------------------------------------"
echo ""

funct_listlogrotatedir

fi

}