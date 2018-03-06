nginxbackup() {

if [ ! -d "$NGINXBACKUPDIR" ]; then
mkdir $NGINXBACKUPDIR
mkdir ${NGINXBACKUPDIR}/confbackup/
mkdir ${NGINXBACKUPDIR}/nginxdirbackup
fi

echo ""
echo "backup NGINX CONF..."
cp -a $NGINXCONFDIR ${NGINXBACKUPDIR}/confbackup/conf_${DT}

echo ""
echo "backup NGINX Install Directory $NGINXDIR..."
cp -a $NGINXDIR ${NGINXBACKUPDIR}/nginxdirbackup/nginx_${DT}

echo ""
echo "---------------------------------------------------------------------------------"
echo "backup of Nginx Install Directory $NGINXDIR located at ${NGINXBACKUPDIR}/nginxdirbackup/nginx_${DT}"
echo "---------------------------------------------------------------------------------"
echo "backup of Nginx CONF located at ${NGINXBACKUPDIR}/confbackup/conf_${DT}"
echo "---------------------------------------------------------------------------------"
echo ""
ls -Alhrt ${NGINXBACKUPDIR}/confbackup/conf_${DT}
echo "---------------------------------------------------------------------------------"
echo ""

}

