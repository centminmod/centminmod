nginxbackup() {

NGXBACKUP_DT=$(date +"%d%m%y-%H%M%S")
# NGINXDIR='/usr/local/nginx'
# NGINXCONFDIR="${NGINXDIR}/conf"
# NGINXBACKUPDIR='/usr/local/nginxbackup'

if [ ! -d "$NGINXBACKUPDIR" ]; then
mkdir $NGINXBACKUPDIR
mkdir ${NGINXBACKUPDIR}/confbackup/
mkdir ${NGINXBACKUPDIR}/nginxdirbackup
fi

echo ""
echo "backup NGINX CONF..."
# cp -a $NGINXCONFDIR ${NGINXBACKUPDIR}/confbackup/conf_${NGXBACKUP_DT}
pushd "$NGINXDIR"
tar -czf "${NGINXBACKUPDIR}/confbackup/nginx-conf-dir-backup-${NGXBACKUP_DT}.tar.gz" conf
ls -lAh "${NGINXBACKUPDIR}/confbackup/nginx-conf-dir-backup-${NGXBACKUP_DT}.tar.gz"
popd

# echo ""
# echo "backup NGINX Install Directory $NGINXDIR..."
# cp -a $NGINXDIR ${NGINXBACKUPDIR}/nginxdirbackup/nginx_${NGXBACKUP_DT}

echo ""
# echo "---------------------------------------------------------------------------------"
# echo "backup of Nginx Install Directory $NGINXDIR located at ${NGINXBACKUPDIR}/nginxdirbackup/nginx_${NGXBACKUP_DT}"
echo "---------------------------------------------------------------------------------"
echo "backup of Nginx CONF located at ${NGINXBACKUPDIR}/confbackup/nginx-conf-dir-backup-${NGXBACKUP_DT}.tar.gz"
echo "---------------------------------------------------------------------------------"
echo ""
ls -Alhrt ${NGINXBACKUPDIR}/confbackup/
echo "---------------------------------------------------------------------------------"
echo ""

}

