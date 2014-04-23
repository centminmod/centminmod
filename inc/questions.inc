questions() {

	if [ "$UNATTENDED" == 'n' ]; then
echo ""
	echo "*********************************************************"
            cecho "  Questions for per app memory usage: Answer y or n and hit enter" $boldgreen
	echo "*********************************************************"
echo ""
read -ep "MySQL: minimise MySQL memory usage ? Recommended if less than <=2048MB memory installed ? [y/n]: " lessmem
echo ""
read -ep "PHP: minimise PHP memory usage ? Recommended if less than <=2048MB memory installed ? [y/n]: " lessphpmem
if [[ "$askxcacheinstall" = [yY] ]];then
echo ""
read -ep "Xcache: minimise Xcache memory usage ? Recommended if less than <=2048MB memory installed ? [y/n]:  " lessxcachemem
fi
if [[ "$askapcinstall" = [yY] ]]; then
echo ""
read -ep "APC Cache: minimise APC Cache memory usage ? Recommended if less than <=2048MB memory installed ? [y/n]:  " lessapcmem
fi
if [[ "$askzopcacheinstall" = [yY] ]]; then
echo ""
read -ep "Zend OPCache: minimise Zend OPcache memory usage ? Recommended if less than <=2048MB memory installed ? [y/n]:  " lesszendopcachemem
fi

	else
	lessmem='y'
	lessphpmem='y'
	lessxcachemem='y'
	lessapcmem='y'
	lesszendopcachemem='y'
	fi #unattended
}