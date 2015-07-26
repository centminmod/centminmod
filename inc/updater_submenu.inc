#######################################################################################
# Centmin Mod code updater sub menu
#######################################################################################
updatersubmenu() {

while :
do
	# clear
        # display menu
	echo
	cecho "--------------------------------------------------------" $boldyellow
	cecho "        Centmin Mod Updater Sub-Menu              " $boldgreen
	cecho "--------------------------------------------------------" $boldyellow
	cecho "1). Setup Centmin Mod Github Environment" $boldgreen
	cecho "2). Update Centmin Mod Current Branch" $boldgreen
	cecho "3). Update Centmin Mod Newer Branch" $boldgreen
	cecho "4). Back to Main menu" $boldgreen
	cecho "--------------------------------------------------------" $boldyellow

	read -ep "Enter option [ 1 - 4 ] " upmoption
	cecho "--------------------------------------------------------" $boldyellow

#########################################################

case "$upmoption" in
1)

centminlog
{
	echo
	cecho "setup Centmin Mod git sourced install..." $boldyellow
	cd /usr/local/src
	
	# wipe /usr/local/src clean of non-git based directories
	rm -rf centminmod*
	rm -rf centminmod-${branchname}
	rm -rf centmin-v1.2.3mod*
	rm -rf centminmod-123.06*
	rm -rf centminmod-123.07*
	rm -rf centminmod-123.08*
	
	echo  
	cecho "download github.com centmin mod ${branchname} branch repo" $boldyellow
	git clone ${CMGIT} centminmod
	cd centminmod
	git checkout -f ${branchname}
	chmod +x centmin.sh
	
	echo
	cecho "list all available local branches" $boldyellow
	cecho "	git branch -a" $boldgreen
	git branch -a
	
	echo
	cecho "list git log last commit" $boldyellow
	cecho "	git log -a" $boldgreen
	git log -1
	
	echo
	cecho "to update centmin mod ${branchname} branch repo via git" $boldyellow
	cecho "	cd /usr/local/src/centminmod" $boldgreen
	cecho "	git stash" $boldgreen
	cecho "	git pull" $boldgreen
	cecho "	chmod +x centmin.sh" $boldgreen
} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_git_envsetup.log

;;
2)

centminlog
{
if [[ -d "${SCRIPT_DIR}/.git" ]]; then
	echo
	cecho "Updating Current Centmin Mod code branch via git" $boldyellow
	cecho "	cd /usr/local/src/centminmod" $boldgreen
	cecho "	git stash" $boldgreen
	cecho "	git pull" $boldgreen
	cecho "	chmod +x centmin.sh" $boldgreen
	cd /usr/local/src/centminmod 2>/dev/null
	git stash
	git pull
	chmod +x centmin.sh
else
	echo
	echo "Error: you do not have git environment setup for github based updates"
	echo "       run submenu option 1 to setup github environment first"
	break
fi
} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_git_updatecur_branch.log
;;
3)

centminlog
{
if [[ -d "${SCRIPT_DIR}/.git" ]]; then
	echo
	cecho "Update Centmin Mod to newer branch via git" $boldyellow
	cecho "You need to input the name of the branch" $boldyellow
	cecho "List of current remote branches by descending date order" $boldyellow
	echo

	for k in `git branch -r|sed s/^..//`;do echo -e `git log -1 --pretty=format:"%Cgreen%ci %Cblue%cr%Creset" "$k" 2>/dev/null`\\t"$k";done| sed -e 's|origin\/||g' -e '/HEAD/d' | sort -r
	
	echo
	read -ep "Enter the branch name you want to switch to i.e. 123.08beta03 : " branchname
	echo

	if [ ! "$(git branch -r | grep "${branchname}$")" ]; then
	   	echo "$branchname branch does not exists"
	   	echo "try again..."
	   	break
	else
		cd /usr/local/src
		# wipe /usr/local/src clean of non-git based directories
		rm -rf centminmod*
		rm -rf centminmod-*
		rm -rf centmin-v1.2.3mod*
		rm -rf centminmod-123.06*
		rm -rf centminmod-123.07*
		rm -rf centminmod-123.08*
	
		echo  
		cecho "download github.com centmin mod ${branchname} branch repo" $boldyellow
		git clone ${CMGIT} centminmod
		cd centminmod 2>/dev/null
		git checkout -f ${branchname}
		chmod +x centmin.sh
		
		echo
		cecho "list all available local branches" $boldyellow
		cecho "	git branch -a" $boldgreen
		git branch -a
		
		echo
		cecho "list git log last commit" $boldyellow
		cecho "	git log -a" $boldgreen
		git log -1
		
		echo
		cecho "to update centmin mod ${branchname} branch repo via git" $boldyellow
		cecho "	cd /usr/local/src/centminmod" $boldgreen
		cecho "	git stash" $boldgreen
		cecho "	git pull" $boldgreen
		cecho "	chmod +x centmin.sh" $boldgreen		
	fi	
else
	echo
	echo "Error: you do not have git environment setup for github based updates"
	echo "       run submenu option 1 to setup github environment first"
	break
fi
} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_git_updatenew_branch.log
;;
4)
	break
;;
esac
done

}

