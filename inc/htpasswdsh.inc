gethtpasswdsh() {
  rm -rf /usr/local/nginx/conf/htpasswd.sh
  wget -cq -O /usr/local/nginx/conf/htpasswd.sh https://github.com/centminmod/centminmod/raw/${branchname}/tools/htpasswd.sh
  chmod +x /usr/local/nginx/conf/htpasswd.sh
  #/usr/local/nginx/conf/htpasswd.sh
}