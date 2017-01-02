#####################################################
# setup TMP_DIR on ramdisk tmpfs
RAMDISKTMPFS='y'

setramdisk() {
	if [[ "$RAMDISKTMPFS" = [yY] ]]; then
		TMPFSLIMIT=4000000
		TMPFSENABLED=1
		if [[ "$TOTALMEM" -ge "$TMPFSLIMIT" ]]; then
			echo "setting up $DIR_TMP on tmpfs ramdisk for initial install"
			mkdir -p $DIR_TMP
			mount -t tmpfs -o size=3100M,mode=0755 tmpfs $DIR_TMP
			df -hT
		else
        	mkdir -p $DIR_TMP
		fi
	else
        mkdir -p $DIR_TMP
	fi
}

unsetramdisk() {
	if [[ "$RAMDISKTMPFS" = [yY] && "$TMPFSENABLED" -eq 1 ]]; then
		echo "unmount $DIR_TMP tmpfs ramdisk & copy back to disk"
		mkdir -p ${DIR_TMP}_disk
		\cp -R ${DIR_TMP}/* ${DIR_TMP}_disk &>/dev/null
		# ls -lah ${DIR_TMP}_disk
		# diff -qr ${DIR_TMP} ${DIR_TMP}_disk &>/dev/null
		umount ${DIR_TMP}
		\cp -R ${DIR_TMP}_disk/* ${DIR_TMP} &>/dev/null
		# ls -lahrt ${DIR_TMP}
		rm -rf ${DIR_TMP}_disk
		df -hT
	fi
}