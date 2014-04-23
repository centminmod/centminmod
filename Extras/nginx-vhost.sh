#!/bin/sh

if [ -z "$1" ]
  then
    echo "Usage: setup-vhost <hostname> (without the www. prefix)"
exit    
fi

# Checking Permissions, making directories, example index.html
mkdir -p /home/nginx/domains/$1/{public,private,log,backup}

cat > "/home/nginx/domains/$1/public/index.html" <<END
<html>
<head>
<title>$1</title>
</head>
<body>
Welcome to $1
</body>
</html>
END

chown -R nginx:nginx "/home/nginx/domains/$1"

# Setting up Nginx mapping
cat > "/usr/local/nginx/conf/conf.d/$1.conf" <<END
server {
  server_name $1 www.$1;

  access_log /home/nginx/domains/$1/log/access.log;
  error_log /home/nginx/domains/$1/log/error.log;

  root /home/nginx/domains/$1/public;

  location / {

  # Enables directory listings when index file not found
  #autoindex  on;

  # Shows file listing times as local time
  #autoindex_localtime on;

  # Enable for vBulletin usage WITHOUT vbSEO installed
  #try_files		$uri $uri/ /index.php;

  }

  include /usr/local/nginx/conf/staticfiles.conf;
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/drop.conf;
}
END

echo 
service nginx reload

echo 
echo vhost for $1 created successfully
echo vhost conf file for $1 created: /usr/local/nginx/conf/conf.d/$1.conf
echo upload files to /home/nginx/domains/$1/public
echo vhost log files directory is /home/nginx/domains/$1/log

echo
echo Current vhost listing at: /usr/local/nginx/conf/conf.d/
ls -Alhrt /usr/local/nginx/conf/conf.d/ | awk '{ printf "%-4s%-4s%-8s%-6s %s\n", $6, $7, $8, $5, $9 }'