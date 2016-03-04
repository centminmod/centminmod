#!/bin/bash
################################################################
# for centminmod.com LEMP stacks to be placed in
# /home/nginx/domains/domain.com/tools/backup-perm.sh
#
# backup nginx vhost site's directory & file permissions
# using
# backup
# getfacl -R < /path/to/filename.acl
# restore
# setfacl --restore=/path/to/filename.acl
################################################################
DT=`date +"%d%m%y-%H%M%S"`
SCRIPTDIR="$(cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd)"
BASEDIR=$(dirname $SCRIPTDIR)
################################################################
if [ ! -d "$BASEDIR/backup/permissions" ]; then
	mkdir -p "$BASEDIR/backup/permissions"
fi
################################################################
backupperm() {
	echo
	echo "-------------------------------------------------------"
	echo " backup directory & file permissions for: "
	echo " $BASEDIR"
	echo "-------------------------------------------------------"
	getfacl -R --absolute-names $BASEDIR > "$BASEDIR/backup/permissions/permissions-$DT.acl"
	ls -lah "$BASEDIR/backup/permissions/permissions-$DT.acl"
	echo "-------------------------------------------------------"
	echo
}
################################################################
restoreperm() {
	echo
	echo "-------------------------------------------------------"
	echo " to restore directory & file permissions for: "
	echo " $BASEDIR"
	echo " find a permission backup file at $BASEDIR/backup/permissions"
	echo " and restore with this command"
	echo "-------------------------------------------------------"
	echo
	echo "setfacl --restore=$BASEDIR/backup/permissions/permissions-XXX.acl"
	echo
	echo "-------------------------------------------------------"
	echo "where permissions-XXX.acl is name of backup"
	echo "-------------------------------------------------------"
	echo
	echo "current backups available are: "
	echo "-------------------------------------------------------"
	ls -rt --format=single-column "$BASEDIR/backup/permissions"
	echo "-------------------------------------------------------"
	echo
}
case "$1" in
	backup)
		backupperm
		;;
	restore)
		restoreperm
		;;
	*)
		echo "$0 {backup|restore}"
		;;
esac
exit