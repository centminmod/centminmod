#######################################################################################
# Zend Opcache sub menu
#######################################################################################

nsdsubmenu() {

while :
do
  # clear
        # display menu
  echo
  cecho "--------------------------------------------------------" $boldyellow
  cecho "            NSD Sub-Menu              " $boldgreen
  cecho "--------------------------------------------------------" $boldyellow
  cecho "1). Install NSD (DNS server)" $boldgreen
  cecho "2). Reinstall NSD (DNS server)" $boldgreen
  cecho "3). Back to Main menu" $boldgreen
  cecho "--------------------------------------------------------" $boldyellow

  read -ep "Enter option [ 1 - 3 ] " nsdoption
  cecho "--------------------------------------------------------" $boldyellow

#########################################################

case "$nsdoption" in
1)
NSD_INSTALL='y'
centminlog
{
nsdinstall
} 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_nsd_install.log"

;;
2)

centminlog
{
nsdreinstall
} 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_nsd_reinstall.log"

;;
3)
  break
;;
esac
done

}

