mysqlproclimit() {

if [[ "$CENTOSVER" = '6.0' || "$CENTOSVER" = '6.1' || "$CENTOSVER" = '6.2' || "$CENTOSVER" = '6.3' || "$CENTOSVER" = '6.4' || "$CENTOSVER" = '6.5' ]]; then

echo ""
echo "-------------------------------------------------------------------------"
echo "Original mysqld process limits: "
echo "cat /proc/`pidof mysqld`/limits"
echo "-------------------------------------------------------------------------"
cat /proc/`pidof mysqld`/limits

echo ""
echo "-------------------------------------------------------------------------"

# if [[ ! -f /etc/security/limits.d/91-mysql.conf ]]; then
echo ""
echo "-------------------------------------------------------------------------"
echo "Increase mysqld process soft limit from 1024 to 9772"
echo "-------------------------------------------------------------------------"
echo "mysql  soft  nproc  9772" > /etc/security/limits.d/91-mysql.conf
echo "Editing /proc/`pidof mysqld`/limits"
echo -n "Max processes=9772:9772" > /proc/`pidof mysqld`/limits
# fi

echo ""
echo "-------------------------------------------------------------------------"

echo ""
echo "-------------------------------------------------------------------------"
echo "New mysqld process limits: "
echo "cat /proc/`pidof mysqld`/limits"
echo "-------------------------------------------------------------------------"
cat /proc/`pidof mysqld`/limits

echo ""
echo "-------------------------------------------------------------------------"

fi

}