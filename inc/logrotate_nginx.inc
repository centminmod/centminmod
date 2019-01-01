# logrotate
# nano -w /etc/logrotate.d/nginx
# logrotate -df /etc/logrotate.d/nginx

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

if [[ ! -f /usr/local/bin/zstd && "$ZSTD_LOGROTATE_NGINX" = [yY] ]]; then
  echo "install zstd compression"
  zstdinstall
fi

touch /etc/logrotate.d/nginx

if [[ "$CENTOS_SEVEN" = '7' ]]; then
        VARDFSIZE=$(df --output=avail /var | tail -1)
else
        VARDFSIZE=$(df -P /var | tail -1 | awk '{print $4}')
fi

if [[ "$TOTALMEM" -le '1153433' || "$VARDFSIZE" -le '10485760' ]]; then
  if [[ -f /usr/local/bin/zstd && "$ZSTD_LOGROTATE_NGINX" = [yY] ]]; then
cat > "/etc/logrotate.d/nginx" <<END
/var/log/nginx/*.log /usr/local/nginx/logs/*.log /home/nginx/domains/*/log/*.log {
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
        /bin/kill -SIGUSR1 \$(cat /usr/local/nginx/logs/nginx.pid 2>/dev/null) 2>/dev/null || true
        endscript           
}
END
  else
cat > "/etc/logrotate.d/nginx" <<END
/var/log/nginx/*.log /usr/local/nginx/logs/*.log /home/nginx/domains/*/log/*.log {
        daily
        dateext
        missingok
        rotate 10
        maxsize 500M
        compress
        delaycompress
        notifempty
        postrotate
        /bin/kill -SIGUSR1 \$(cat /usr/local/nginx/logs/nginx.pid 2>/dev/null) 2>/dev/null || true
        endscript           
}
END
  fi
else
  if [[ -f /usr/local/bin/zstd && "$ZSTD_LOGROTATE_NGINX" = [yY] ]]; then
cat > "/etc/logrotate.d/nginx" <<END
/var/log/nginx/*.log /usr/local/nginx/logs/*.log /home/nginx/domains/*/log/*.log {
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
        /bin/kill -SIGUSR1 \$(cat /usr/local/nginx/logs/nginx.pid 2>/dev/null) 2>/dev/null || true
        endscript           
}
END
  else
cat > "/etc/logrotate.d/nginx" <<END
/var/log/nginx/*.log /usr/local/nginx/logs/*.log /home/nginx/domains/*/log/*.log {
        daily
        dateext
        missingok
        rotate 10
        compress
        delaycompress
        notifempty
        postrotate
        /bin/kill -SIGUSR1 \$(cat /usr/local/nginx/logs/nginx.pid 2>/dev/null) 2>/dev/null || true
        endscript           
}
END
  fi
fi

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