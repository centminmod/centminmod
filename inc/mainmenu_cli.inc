climainmenu() {
case "$1" in
install)

lowmemcheck
diskalert
alldownloads
funct_centmininstall
echo "$SCRIPT_VERSION" > /etc/centminmod-release
echo "$SCRIPT_VERSION #`date`" >> /etc/centminmod-versionlog

;;
addvhost)

funct_nginxaddvhost

;;
nsdsetup)

funct_nsdsetup

;;
nginxupgrade)

diskalert
csftweaks
yuminstall
funct_nginxupgrade

;;
phpupgrade)

diskalert
csftweaks
yuminstall
funct_phpupgrade

;;
xcachereinstall)

funct_xcachereinstall

;;
apcreinstall)

funct_apcreinstall

;;
installxcache)

funct_installxcache

;;
installapc)

funct_installapc

;;
memcachedreinstall)

funct_memcachedreinstall

;;
mariadbupgrade)

mariadbsubmenu

;;
iopinginstall)

funct_installiopingcentmin

;;
selinux)

funct_selinux

;;
logrotate)

funct_logrotate

;;
phplogrotate)

funct_logphprotate

;;
sshdport)

funct_sshd

;;
multithreadcomp)

funct_pigzinstall
funct_pbzip2install
funct_lbzip2install
funct_lzipinstall
funct_plzipinstall
funct_p7zipinstall

;;
suhosininstall)
suhosinsetup
;;
ffmpeginstall)
ffmpegsetup
;;
nsdreinstall)
nsdreinstall
;;
exit)

echo ""
echo "exit"

bookmark

exit 0

;;
*)

echo "$0 install"
echo "$0 addvhost"
echo "$0 nsdsetup"
echo "$0 nginxupgrade"
echo "$0 phpupgrade"
echo "$0 xcachereinstall"
echo "$0 apcreinstall"
echo "$0 installxcache"
echo "$0 installapac"
echo "$0 memcachedreinstall"
echo "$0 mariadbupgrade"
echo "$0 installioping"
echo "$0 selinux"
echo "$0 logrotate"
echo "$0 phplogrotate"
echo "$0 sshdport"
echo "$0 multithreadcomp"
echo "$0 suhosininstall"
echo "$0 ffmpeginstall"
echo "$0 nsdreinstall"

;;
esac
} #climainmenu