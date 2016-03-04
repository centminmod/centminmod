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

DELOLD='y'
DELOLD_VERBOSE='n'
DEL_THRESHOLD='365'
################################################################
if [ ! -d "$BASEDIR/backup/permissions" ]; then
	mkdir -p "$BASEDIR/backup/permissions"
fi
################################################################
cleanup() {
if [[ "$DELOLD" = [yY] ]]; then
	find "$BASEDIR/backup/permissions" -maxdepth 1 -mtime +$DEL_THRESHOLD | sort | while read BACKUPFILE; do
		if [[ "$DELOLD_VERBOSE" = [yY] ]]; then
			echo "    Deleting older than $DEL_THRESHOLD days backup: $BACKUPFILE"
			echo "    rm -rf $BACKUPFILE"
			rm -rf $BACKUPFILE
		else
			rm -rf $BACKUPFILE
		fi
	done
fi
}

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
		cleanup
		;;
	restore)
		restoreperm
		;;
	*)
		echo "$0 {backup|restore}"
		;;
esac
exit