#!/bin/bash
################################################################
# 1, for centminmod.com LEMP stacks to be placed in
# /home/nginx/domains/domain.com/tools/backup-perm.sh
#
# 2. setup cronjob to run every 6 hours at 11th minute
# 11 */6 * * * /home/nginx/domains/domain.com/tools/backup-perm.sh backup 2>&1 /dev/null
#
# this will backup nginx vhost site's directory & file permissions
# using
# 
# backup
# getfacl -R -L --absolute-names /home/nginx/domains/domain.com > /path/to/filename.acl
# 
# restore
# setfacl --restore=/path/to/filename.acl
################################################################
DT=$(date +"%d%m%y-%H%M%S")
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
# set locale temporarily to english
# due to some non-english locale issues
export LC_ALL=en_US.UTF-8
export LANG=en_US.UTF-8
export LANGUAGE=en_US.UTF-8
export LC_CTYPE=en_US.UTF-8

shopt -s expand_aliases
for g in "" e f; do
    alias ${g}grep="LC_ALL=C ${g}grep"  # speed-up grep, egrep, fgrep
done

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

exec_nginxperm() {
	echo "/usr/bin/chown -R nginx:nginx $BASEDIR"
	/usr/bin/chown -R nginx:nginx "$BASEDIR"
	echo
	ls -lah "$BASEDIR"
}

backupperm() {
	echo
	echo "-------------------------------------------------------"
	echo " backup directory & file permissions for: "
	echo " $BASEDIR"
	echo "-------------------------------------------------------"
	getfacl -R -L --absolute-names $BASEDIR > "$BASEDIR/backup/permissions/permissions-$DT.acl"
	ls -lah "$BASEDIR/backup/permissions/permissions-$DT.acl"
	echo "-------------------------------------------------------"
	echo
	echo "ensure directory and files have correct nginx user:group permission"
	exec_nginxperm
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