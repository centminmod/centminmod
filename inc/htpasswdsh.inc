gethtpasswdsh() {
rm -rf /usr/local/nginx/conf/htpasswd.sh
wget -cq -O /usr/local/nginx/conf/htpasswd.sh https://gist.githubusercontent.com/centminmod/3068afbed67d5fe5e2dd/raw
chmod +x /usr/local/nginx/conf/htpasswd.sh
#/usr/local/nginx/conf/htpasswd.sh
}