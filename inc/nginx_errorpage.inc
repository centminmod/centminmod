function funct_nginxerrorpages {

echo "*************************************************"
cecho "* Custom error page setup for Nginx " $boldgreen
echo "*************************************************"

echo "checking if /usr/local/nginx/conf/errorpage.conf"
echo "and custom error pages already exists"
echo "before setting up /usr/local/nginx/conf/errorpage.conf"
echo ""

if [[ ! -f /usr/local/nginx/html/404.html && ! -f /usr/local/nginx/conf/errorpage.conf ]]; then

echo "/usr/local/nginx/conf/errorpage.conf and custom error pages don't exist"
echo "setting up errorpage.conf and custom error pages ..."
echo ""

cp -R $CUR_DIR/htdocs/custom_errorpages/* /usr/local/nginx/html
cp $CUR_DIR/config/nginx/errorpage.conf /usr/local/nginx/conf

sed -i 's/include \/usr\/local\/nginx\/conf\/drop.conf;/include \/usr\/local\/nginx\/conf\/drop.conf;\n#include \/usr\/local\/nginx\/conf\/errorpage.conf;/g' /usr/local/nginx/conf/conf.d/*.conf

else

echo "/usr/local/nginx/conf/errorpage.conf and custom error pages"
echo "already exists ... aborting"
echo ""

exit

fi

echo "*************************************************"

}
