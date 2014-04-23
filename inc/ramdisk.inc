#####################################################
# setup TMP_DIR on ramdisk tmpfs
RAMDISKTMPFS='y'

setramdisk() {
	if [[ "$RAMDISKTMPFS" = [yY] ]]; then
		TMPFSLIMIT=2900000
		TMPFSENABLED=1
		if [[ "$TOTALMEM" -ge "$TMPFSLIMIT" ]]; then
			echo "setting up $DIR_TMP on tmpfs ramdisk for initial install"
			mkdir -p $DIR_TMP
			mount -t tmpfs -o size=2200M,mode=0755 tmpfs $DIR_TMP
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
		\cp -R ${DIR_TMP}/* ${DIR_TMP}_disk
		# ls -lah ${DIR_TMP}_disk
		diff -qr ${DIR_TMP} ${DIR_TMP}_disk
		umount ${DIR_TMP}
		\cp -R ${DIR_TMP}_disk/* ${DIR_TMP}
		# ls -lahrt ${DIR_TMP}
		df -hT
	fi
}