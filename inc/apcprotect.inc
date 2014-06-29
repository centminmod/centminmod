apcprotect() {

cat > protectapc.txt << EOF
To password protect your ${N}_myapc.php file at /usr/local/nginx/html
you need to do 3 steps:

#############################################################
1). Use htpasswd.py script at /usr/local/nginx/conf/htpasswd.py to setup
username and password using following command:

  python /usr/local/nginx/conf/htpasswd.py -b /usr/local/nginx/conf/htpasswd yourusername yourpassword

change yourusername and yourpassword to unique values of your own.

#############################################################
2). Add the following 6 lines of code to your main nginx virtual host file 
/usr/local/nginx/conf/conf.d/virtual.conf first server{} entry.

6 lines of code:

location ~ ^/${N}_myapc\.php$ {
auth_basic "Private";
auth_basic_user_file /usr/local/nginx/conf/htpasswd;
include /usr/local/nginx/conf/staticfiles.conf;
include /usr/local/nginx/conf/php.conf;
}

End result for first server{} entry looks like below:

server {
            listen   80;
            server_name localhost;
            root   html;

# limit_conn limit_per_ip 16;
# ssi  on;

            location / {

#               Enables directory listings when index file not found
#               autoindex  on;

#               Shows file listing times as local time
#               autoindex_localtime on;

#               Enable for vBulletin usage WITHOUT vbSEO installed
#               try_files               $uri $uri/ /index.php;

            }

location ~ ^/${N}_myapc\.php$ {
auth_basic "Private";
auth_basic_user_file /usr/local/nginx/conf/htpasswd;
include /usr/local/nginx/conf/staticfiles.conf;
include /usr/local/nginx/conf/php.conf;
}

include /usr/local/nginx/conf/staticfiles.conf;
include /usr/local/nginx/conf/php.conf;
include /usr/local/nginx/conf/drop.conf;
#include /usr/local/nginx/conf/errorpage.conf;

       }

#############################################################
3). Restart Nginx

service nginx restart

EOF

cat protectapc.txt

echo ""
cecho "Important instructions above to protect your apc.php page" $boldgreen

	if [ "$UNATTENDED" == 'n' ]; then
	cecho "install will continue in 80 seconds..." $boldgreen
	else
	cecho "install will continue in 10 seconds..." $boldgreen
	fi #unattended

	if [ "$UNATTENDED" == 'n' ]; then
	sleep 30
	else
	sleep 4
	fi #unattended

}