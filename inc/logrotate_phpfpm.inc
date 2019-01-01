# logrotate
# nano -w /etc/logrotate.d/php-fpm
# logrotate -df /etc/logrotate.d/php-fpm

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

if [[ ! -f /usr/local/bin/zstd && "$ZSTD_LOGROTATE_PHPFPM" = [yY] ]]; then
  echo "install zstd compression"
  zstdinstall
fi

touch /etc/logrotate.d/php-fpm

if [[ "$CENTOS_SEVEN" = '7' ]]; then
        VARDFSIZE=$(df --output=avail /var | tail -1)
else
        VARDFSIZE=$(df -P /var | tail -1 | awk '{print $4}')
fi

if [[ "$TOTALMEM" -le '1153433' || "$VARDFSIZE" -le '10485760' ]]; then
  if [[ -f /usr/local/bin/zstd && "$ZSTD_LOGROTATE_PHPFPM" = [yY] ]]; then
cat > "/etc/logrotate.d/php-fpm" <<END
/var/log/php-fpm/*.log {
        daily
        dateext
        missingok
        rotate 10
        maxsize 500M
        compress
        delaycompress
        compresscmd /usr/local/bin/zstd
        uncompresscmd /usr/local/bin/unzstd
        compressoptions -9 --long -T0
        compressext .zst
        notifempty
        postrotate
        /bin/kill -SIGUSR1 \$(cat /var/run/php-fpm/php-fpm.pid 2>/dev/null) 2>/dev/null || true
        endscript            
}
END
  else
cat > "/etc/logrotate.d/php-fpm" <<END
/var/log/php-fpm/*.log {
        daily
        dateext
        missingok
        rotate 10
        maxsize 500M
        compress
        delaycompress
        notifempty
        postrotate
        /bin/kill -SIGUSR1 \$(cat /var/run/php-fpm/php-fpm.pid 2>/dev/null) 2>/dev/null || true
        endscript            
}
END
  fi
else
  if [[ -f /usr/local/bin/zstd && "$ZSTD_LOGROTATE_PHPFPM" = [yY] ]]; then
cat > "/etc/logrotate.d/php-fpm" <<END
/var/log/php-fpm/*.log {
        daily
        dateext
        missingok
        rotate 10
        compress
        delaycompress
        compresscmd /usr/local/bin/zstd
        uncompresscmd /usr/local/bin/unzstd
        compressoptions -9 --long -T0
        compressext .zst
        notifempty
        postrotate
        /bin/kill -SIGUSR1 \$(cat /var/run/php-fpm/php-fpm.pid 2>/dev/null) 2>/dev/null || true
        endscript            
}
END
  else
cat > "/etc/logrotate.d/php-fpm" <<END
/var/log/php-fpm/*.log {
        daily
        dateext
        missingok
        rotate 10
        compress
        delaycompress
        notifempty
        postrotate
        /bin/kill -SIGUSR1 \$(cat /var/run/php-fpm/php-fpm.pid 2>/dev/null) 2>/dev/null || true
        endscript            
}
END
  fi
fi

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