diskalert() {
if [ ! -f /etc/cron.daily/diskalert ]; then
cp $CUR_DIR/cron/diskalert /etc/cron.daily/
chmod +x /etc/cron.daily/diskalert
fi
}