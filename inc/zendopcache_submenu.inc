#######################################################################################
# Zend Opcache sub menu
#######################################################################################

zendoptcachesubmenu() {

while :
do
	# clear
        # display menu
	echo
	cecho "--------------------------------------------------------" $boldyellow
	cecho "        Zend OpCache Sub-Menu              " $boldgreen
	cecho "--------------------------------------------------------" $boldyellow
	cecho "1). Install Zend OpCache for PHP <5.5 or <5.6" $boldgreen
	cecho "2). Reinstall Zend OpCache for PHP <5.5 or <5.6" $boldgreen
	cecho "3). Back to Main menu" $boldgreen
	cecho "--------------------------------------------------------" $boldyellow

	read -ep "Enter option [ 1 - 3 ] " zopoption
	cecho "--------------------------------------------------------" $boldyellow

#########################################################

case "$zopoption" in
1)

centminlog
{
zopcacheinstall
} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_zendopcache_install_submenu.log

;;
2)

centminlog
{
zopcachereinstall
} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_zendopcache_reinstall_submenu.log

;;
3)
	break
;;
esac
done

}

