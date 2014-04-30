function funct_nginxaddvhost {

echo ""
cecho "---------------------------------------------" $boldyellow

read -ep "Enter vhost domain name you want to add (without www. prefix): " vhostname

echo ""

if [ ! -d /home/nginx/domains/$vhostname ]; then

# Checking Permissions, making directories, example index.html
mkdir -p /home/nginx/domains/$vhostname/{public,private,log,backup}

cat > "/home/nginx/domains/$vhostname/public/index.html" <<END
<html>
<head>
<title>$vhostname</title>
</head>
<body>
<p>Welcome to $vhostname. This index.html page can be removed.</p>

<p>Useful Centmin Mod info and links to bookmark.</p>

<ul>
  <li>Getting Started Guide - <a href="http://centminmod.com/getstarted.html" target="_blank">http://centminmod.com/getstarted.html</a></li>
  <li>Latest Centmin Mod version - <a href="http://centminmod.com" target="_blank">http://centminmod.com</a></li>
  <li>Centmin Mod FAQ - <a href="http://centminmod.com/faq.html" target="_blank">http://centminmod.com/faq.html</a></li>
  <li>Change Log - <a href="http://centminmod.com/changelog.html" target="_blank">http://centminmod.com/changelog.html</a></li>
  <li>Google+ Page latest news <a href="http://centminmod.com/gpage" target="_blank">http://centminmod.com/gpage</a></li>
  <li>Google+ Community Forum <a href="http://centminmod.com/gcom" target="_blank">http://centminmod.com/gcom</a></li>
  <li>Centmin Mod Twitter <a href="https://twitter.com/centminmod" target="_blank">https://twitter.com/centminmod</a></li>
</ul>

<p><a href="https://www.digitalocean.com/?refcode=c1cb367108e8" target="_blank">Cheap VPS Hosting at Digitalocean</a></p>

</body>
</html>
END

    cp -R $CUR_DIR/htdocs/custom_errorpages/* /home/nginx/domains/$vhostname/public

chown -R nginx:nginx "/home/nginx/domains/$vhostname"
find "/home/nginx/domains/$vhostname" -type d -exec chmod g+s {} \;

# Setting up Nginx mapping
cat > "/usr/local/nginx/conf/conf.d/$vhostname.conf" <<END
# redirect from non-www to www 
# uncomment, save file and restart Nginx to enable
#server {
#            listen   80;
#            server_name $vhostname;
#            return 301 \$scheme://www.${vhostname}\$request_uri;
#       }

server {
  server_name $vhostname www.$vhostname;

# ngx_pagespeed & ngx_pagespeed handler
#include /usr/local/nginx/conf/pagespeed.conf;
#include /usr/local/nginx/conf/pagespeedhandler.conf;
#include /usr/local/nginx/conf/pagespeedstatslog.conf;

  # limit_conn limit_per_ip 16;
  # ssi  on;

  access_log /home/nginx/domains/$vhostname/log/access.log combined buffer=32k;
  error_log /home/nginx/domains/$vhostname/log/error.log;

  root /home/nginx/domains/$vhostname/public;

  location / {

# block common exploits, sql injections etc
#include /usr/local/nginx/conf/block.conf;

  # Enables directory listings when index file not found
  #autoindex  on;

  # Shows file listing times as local time
  #autoindex_localtime on;

  # Enable for vBulletin usage WITHOUT vbSEO installed
  #try_files		\$uri \$uri/ /index.php;

  }

  include /usr/local/nginx/conf/staticfiles.conf;
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/drop.conf;
  #include /usr/local/nginx/conf/errorpage.conf;
}
END

echo 
cecho "---------------------------------------------" $boldyellow
service nginx reload

echo 
cecho "---------------------------------------------" $boldyellow
cecho "vhost for $vhostname created successfully" $boldwhite
cecho "vhost conf file for $vhostname created: /usr/local/nginx/conf/conf.d/$vhostname.conf" $boldwhite
cecho "upload files to /home/nginx/domains/$vhostname/public" $boldwhite
cecho "vhost log files directory is /home/nginx/domains/$vhostname/log" $boldwhite

echo
cecho "Current vhost listing at: /usr/local/nginx/conf/conf.d/" $boldwhite

ls -Alhrt /usr/local/nginx/conf/conf.d/ | awk '{ printf "%-4s%-4s%-8s%-6s %s\n", $6, $7, $8, $5, $9 }'

cecho "---------------------------------------------" $boldyellow

else

echo ""
cecho "---------------------------------------------" $boldyellow
cecho "vhost for $vhostname already exists" $boldwhite
cecho "/home/nginx/domains/$vhostname already exists" $boldwhite
cecho "---------------------------------------------" $boldyellow
echo ""

fi


}