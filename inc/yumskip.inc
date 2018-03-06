yumskipinstall() {

echo ""
echo -ne "Do you want to run YUM install checks ?  [y/n]\n\nThis will increase your upgrade duration time wise.\nCheck the change log centminmod.com/changelog.html\nto see if any Nginx or PHP related new additions\nwhich require checking YUM prequisites are met.\nIf no new additions made, you can skip the \nYUM install check to speed up upgrade time.\n\n [y/n]: " 
read yuminstallrun

}