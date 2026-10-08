nginxbackup() {

NGXBACKUP_DT=$(date +"%d%m%y-%H%M%S")
# NGINXDIR='/usr/local/nginx'
# NGINXCONFDIR="${NGINXDIR}/conf"
# NGINXBACKUPDIR='/usr/local/nginxbackup'

mkdir -p "${NGINXBACKUPDIR}/confbackup" || return 1

echo ""
echo "backup NGINX CONF..."
# cp -a $NGINXCONFDIR ${NGINXBACKUPDIR}/confbackup/conf_${NGXBACKUP_DT}
if ! tar -C "$NGINXDIR" -czf "${NGINXBACKUPDIR}/confbackup/nginx-conf-dir-backup-${NGXBACKUP_DT}.tar.gz" conf; then
    rm -f "${NGINXBACKUPDIR}/confbackup/nginx-conf-dir-backup-${NGXBACKUP_DT}.tar.gz"
    return 1
fi
ls -lAh "${NGINXBACKUPDIR}/confbackup/nginx-conf-dir-backup-${NGXBACKUP_DT}.tar.gz"

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

