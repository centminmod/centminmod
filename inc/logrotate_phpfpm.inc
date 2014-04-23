# logrotate
# nano -w /etc/logrotate.d/php-fpm

function funct_listphplogrotatedir {

echo "---------------------------------------------------------------------------"
echo "Current php-fpm log directories:"
echo "---------------------------------------------------------------------------"
if [ -d /var/log/php-fpm/ ]; then
echo "ls -lh /var/log/php-fpm/"
ls -lh /var/log/php-fpm/
echo "---------------------------------------------------------------------------"
fi

}

function funct_logphprotate {

if [ ! -f /etc/logrotate.d/php-fpm ]; then

echo "----------------------------------------------"
echo "Creating /etc/logrotate.d/php-fpm file"
echo "----------------------------------------------"
echo "......"

touch /etc/logrotate.d/php-fpm

cat > "/etc/logrotate.d/php-fpm" <<END
/var/log/php-fpm/*.log {
        daily
        missingok
        rotate 10
        size=100M
        compress
        delaycompress
        notifempty
        postrotate
                /sbin/service php-fpm restart
               #or
               #kill -s USR1 'cat /var/run/php-fpm/php-fpm.pid'
        endscript            
}
END

echo "----------------------------------------------------------------------------"
cat /etc/logrotate.d/php-fpm

funct_listphplogrotatedir

echo "----------------------------------------------------------------------------"
echo "php-fpm logrotation /etc/logrotate.d/php-fpm setup completed"
echo "----------------------------------------------------------------------------"

else 

echo "--------------------------------------------------"
echo " /etc/logrotate.d/php-fpm already exists"
echo "--------------------------------------------------"
echo ""

funct_listphplogrotatedir

fi

}