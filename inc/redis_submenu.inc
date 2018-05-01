#######################################################################################
# Redis PHP extension sub menu
#######################################################################################

phpredis_submenu() {

while :
do
  # clear
        # display menu
  echo
  cecho "--------------------------------------------------------" $boldyellow
  cecho "         Redis PHP Extension Sub-Menu              " $boldgreen
  cecho "--------------------------------------------------------" $boldyellow
  cecho "1). Install Redis PHP Extension" $boldgreen
  cecho "2). Reinstall Redis PHP Extension" $boldgreen
  cecho "3). Back to Main menu" $boldgreen
  cecho "--------------------------------------------------------" $boldyellow

  read -ep "Enter option [ 1 - 3 ] " phpredisoption
  cecho "--------------------------------------------------------" $boldyellow

#########################################################

case "$phpredisoption" in
1)
PHPREDIS='y'
centminlog
{
if [[ "$TIME_REDIS" = [yY] ]]; then
  redisinstall submenu 2>&1 | awk '{ print strftime("%Y-%m-%d %H:%M:%S"), $0; fflush(); }'
else
  redisinstall submenu
fi
/usr/bin/nprestart >/dev/null 2>&1
} 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_redis-php-install.log"

;;
2)

centminlog
{
PHPREDIS='y'
if [[ "$TIME_REDIS" = [yY] ]]; then
  redisinstall submenu 2>&1 | awk '{ print strftime("%Y-%m-%d %H:%M:%S"), $0; fflush(); }'
else
  redisinstall submenu
fi
/usr/bin/nprestart >/dev/null 2>&1
} 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_redis-php-reinstall.log"

;;
3)
  break
;;
esac
done

}

