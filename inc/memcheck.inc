# If set to yes, will abort centmin mod installation if <256MB system memory is detected
# you can override this setting by setting ABORTINSTALL='n' in which case centmin mod
# install on <256MB system will either install successfully but very very slowly or crap out
# and fail to successfully install. I have tested on 112MB memory system, and centmin mod
# install swapped to disk by ~68MB, so you need at least 192MB and adequate swap disk size
ABORTINSTALL='y'
#############################################################
TOTALMEM=$(cat /proc/meminfo | grep MemTotal | awk '{print $2}')
ISLOWMEM='262144'
#############################################################
# Formulas
TOTALMEMMB=`echo "scale=0;$TOTALMEM/1024" | bc`
ISLOWMEMMB=`echo "scale=0;$ISLOWMEM/1024" | bc`
CHECKLOWMEM=`expr $TOTALMEM \< $ISLOWMEM`
#############################################################

lowmemcheck() {

cpcheck

# Check memory installed threshold
if [ "$CHECKLOWMEM" == "1" ]; then
CPUS='1'
MAKETHREADS=" -j$CPUS"
echo ""
echo -e "Warning: physically installed memory too low for Centmin Mod\nInstallation [Installed: $TOTALMEMMB MB < $ISLOWMEMMB MB (recommended minimum)]\n"

if [ "$ABORTINSTALL" == 'y' ]; then
echo "aborting install..."
sleep 20
exit
fi

else
echo ""
echo -e "Ok: physically installed memory is sufficient for Centmin Mod\nInstallation [Installed: $TOTALMEMMB MB]\n"

fi

}