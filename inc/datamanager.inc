#######################################################################################
# Data Management Routines
#######################################################################################

#######################################################################################
check_for_awscli_get() {
  if [[ -d "$SCRIPT_DIR/datamanagement" && ! -f "$SCRIPT_DIR/datamanagement/awscli-get.sh" ]]; then
    curl -4sL https://awscli-get.centminmod.com/awscli-get.sh -o "$SCRIPT_DIR/datamanagement/awscli-get.sh"
    chmod +x "$SCRIPT_DIR/datamanagement/awscli-get.sh"
  fi
  if [[ -d "$SCRIPT_DIR/datamanagement" && -f "$SCRIPT_DIR/datamanagement/awscli-get.sh" ]]; then
    awscli_get_found=y
  else
    awscli_get_found=n
    ls -lAh "$SCRIPT_DIR/datamanagement/awscli-get.sh"
    echo
    echo "awscli-get.sh not found"
    exit 1
  fi
}

#######################################################################################
check_for_keygen() {
  if [[ -d "$SCRIPT_DIR/datamanagement" && ! -f "$SCRIPT_DIR/datamanagement/keygen.sh" ]]; then
    curl -4sL https://github.com/centminmod/keygen/raw/master/keygen.sh -o "$SCRIPT_DIR/datamanagement/keygen.sh"
    chmod +x "$SCRIPT_DIR/datamanagement/keygen.sh"
  fi
  if [[ -d "$SCRIPT_DIR/datamanagement" && -f "$SCRIPT_DIR/datamanagement/keygen.sh" ]]; then
    keygen_found=y
  else
    keygen_found=n
    ls -lAh "$SCRIPT_DIR/datamanagement/keygen.sh"
    echo
    echo "keygen.sh not found"
    exit 1
  fi
}

#######################################################################################
aws_profiles_submenu() {
while :
do
  # clear
        # display menu
  echo
  cecho "--------------------------------------------------------" $boldyellow
  cecho "     Manage SSH Keys        " $boldgreen
  cecho "--------------------------------------------------------" $boldyellow
  cecho "1).   List Registered AWS CLI S3 Profiles" $boldgreen
  cecho "2).   List AWS CLI S3 Profile Configuration" $boldgreen
  cecho "3).   Create New AWS CLI S3 Profile" $boldgreen
  cecho "4).   Edit Existing AWS CLI S3 Profile" $boldgreen
  cecho "5).   Delete Existing AWS CLI S3 Profile" $boldgreen
  cecho "6).   Export Existing AWS CLI S3 Profile" $boldgreen
  cecho "7).   Backup All Existing AWS CLI S3 Profiles" $boldgreen
  cecho "8).   Back to Main menu" $boldgreen
  cecho "--------------------------------------------------------" $boldyellow

  read -ep "Enter option [ 1 - 8 ] " awscli_profiles_menu_opt
  cecho "--------------------------------------------------------" $boldyellow

#########################################################

case "$awscli_profiles_menu_opt" in
1)
  {
    check_for_awscli_get
    echo
    echo "List Registered AWS CLI S3 Profiles"
    echo

  } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_${awscli_profiles_menu_opt}_awscli_profiles_menu.log"
;;
2)
  {
    check_for_awscli_get
    echo
    echo "List AWS CLI S3 Profile Configuration"
    echo

  } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_${awscli_profiles_menu_opt}_awscli_profiles_menu.log"
;;
3)
  {
    check_for_awscli_get
    echo
    echo "Create New AWS CLI S3 Profile"
    echo

  } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_${awscli_profiles_menu_opt}_awscli_profiles_menu.log"
;;
4)
  {
    check_for_awscli_get
    echo
    echo "Edit Existing AWS CLI S3 Profile"
    echo

  } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_${awscli_profiles_menu_opt}_awscli_profiles_menu.log"
;;
5)
  {
    check_for_awscli_get
    echo
    echo "Delete Existing AWS CLI S3 Profile"
    echo

  } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_${awscli_profiles_menu_opt}_awscli_profiles_menu.log"
;;
6)
  {
    check_for_awscli_get
    echo
    echo "Export Existing AWS CLI S3 Profile"
    echo

  } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_${awscli_profiles_menu_opt}_awscli_profiles_menu.log"
;;
7)
  {
    check_for_awscli_get
    echo
    echo "Backup All Existing AWS CLI S3 Profiles"
    echo

  } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_${awscli_profiles_menu_opt}_awscli_profiles_menu.log"
;;
8)
  break
;;
esac
done
}

#######################################################################################
ssh_keys_submenu() {
while :
do
  # clear
        # display menu
  echo
  cecho "--------------------------------------------------------" $boldyellow
  cecho "     Manage SSH Keys        " $boldgreen
  cecho "--------------------------------------------------------" $boldyellow
  cecho "1).   List Registered SSH Keys" $boldgreen
  cecho "2).   Register Existing SSH Keys" $boldgreen
  cecho "3).   Create New SSH Key For Remote Host" $boldgreen
  cecho "4).   Use Existing SSH Key For Remote Host" $boldgreen
  cecho "5).   Rotate Existing SSH Key For Remote Host" $boldgreen
  cecho "6).   Delete Existing SSH Key For Remote Host" $boldgreen
  cecho "7).   Export Existing SSH Key" $boldgreen
  cecho "8).   Backup All Existing SSH Keys" $boldgreen
  cecho "9).   Back to Main menu" $boldgreen
  cecho "--------------------------------------------------------" $boldyellow

  read -ep "Enter option [ 1 - 9 ] " sshkeys_menu_opt
  cecho "--------------------------------------------------------" $boldyellow

#########################################################

case "$sshkeys_menu_opt" in
1)
  {
    check_for_keygen
    echo
    echo "List Registered SSH Keys"
    echo

  } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_${sshkeys_menu_opt}_sshkeys_menu.log"
;;
2)
  {
    check_for_keygen
    echo
    echo "Register Existing SSH Keys"
    echo

  } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_${sshkeys_menu_opt}_sshkeys_menu.log"
;;
3)
  {
    check_for_keygen
    echo
    echo "Create New SSH Key For Remote Host"
    echo

  } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_${sshkeys_menu_opt}_sshkeys_menu.log"
;;
4)
  {
    check_for_keygen
    echo
    echo "Use Existing SSH Key For Remote Host"
    echo

  } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_${sshkeys_menu_opt}_sshkeys_menu.log"
;;
5)
  {
    check_for_keygen
    echo
    echo "Rotate Existing SSH Key For Remote Host"
    echo

  } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_${sshkeys_menu_opt}_sshkeys_menu.log"
;;
6)
  {
    check_for_keygen
    echo
    echo "Delete Existing SSH Key For Remote Host"
    echo

  } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_${sshkeys_menu_opt}_sshkeys_menu.log"
;;
7)
  {
    check_for_keygen
    echo
    echo "Export Existing SSH Key"
    echo

  } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_${sshkeys_menu_opt}_sshkeys_menu.log"
;;
8)
  {
    check_for_keygen
    echo
    echo "Backup All Existing SSH Keys"
    echo

  } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_${sshkeys_menu_opt}_sshkeys_menu.log"
;;
9)
  break
;;
esac
done
}

#######################################################################################
check_for_datatools() {
  if [[ -d "$SCRIPT_DIR/datamanagement" && -f "$SCRIPT_DIR/datamanagement/backups.sh" && -f "$SCRIPT_DIR/datamanagement/mariabackup-restore.sh" && -f "$SCRIPT_DIR/datamanagement/tunnel-transfers.sh" ]]; then
    datatools_found=y
  else
    datatools_found=n
    ls -lAh "$SCRIPT_DIR/datamanagement/backups.sh"
    ls -lAh "$SCRIPT_DIR/datamanagement/mariabackup-restore.sh"
    ls -lAh "$SCRIPT_DIR/datamanagement/tunnel-transfers.sh"
    echo
    echo "Data management tools not found"
    exit 1
  fi
}

#######################################################################################
datamanager_menu() {
while :
do
  # clear
        # display menu
  echo
  cecho "--------------------------------------------------------" $boldyellow
  cecho "     Centmin Mod Data Management        " $boldgreen
  cecho "--------------------------------------------------------" $boldyellow
  cecho "1).   Manage SSH Keys" $boldgreen
  cecho "2).   Manage AWS CLI S3 Profile Credentials" $boldgreen
  cecho "3).   Migrate Centmin Mod Data To New Centmin Mod Server" $boldgreen
  cecho "4).   Backup Nginx Vhosts Data + MariaBackup MySQL Backups" $boldgreen
  cecho "5).   Backup Nginx Vhosts Data Only (no MariaDB MySQL backups)" $boldgreen
  cecho "6).   Backup MariaDB MySQL With MariaBackup Only (no Vhosts Data backups)" $boldgreen
  cecho "7).   Backup MariaDB MySQL With mysqldump only (no Vhosts Data backups)" $boldgreen
  cecho "8).   Transfer Directory Data To Remote Server Via SSH" $boldgreen
  cecho "9).   Transfer Directory Data To S3 Compatible Storage" $boldgreen
  cecho "10).  Transfer Files To S3 Compatible Storage" $boldgreen
  cecho "11).  Download S3 Compatible Stored Data To Server" $boldgreen
  cecho "12).  S3 To S3 Compatible Storage Transfers" $boldgreen
  cecho "13).  List S3 Storage Buckets" $boldgreen
  cecho "14).  Back to Main menu" $boldgreen
  cecho "--------------------------------------------------------" $boldyellow

  read -ep "Enter option [ 1 - 14 ] " datamenu_opt
  cecho "--------------------------------------------------------" $boldyellow

#########################################################

case "$datamenu_opt" in
1)
  {
    check_for_datatools
    echo
    echo "Manage SSH Keys"
    echo
    ssh_keys_submenu

  } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_${datamenu_opt}_datamanger_menu.log"
;;
2)
  {
    check_for_datatools
    echo
    echo "Manage AWS CLI S3 Profile Credentials"
    echo
    aws_profiles_submenu

  } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_${datamenu_opt}_datamanger_menu.log"
;;
3)
  {
    check_for_datatools
    echo
    echo "Migrate Centmin Mod Data To New Centmin Mod Server"
    echo
    echo "Description:"
    echo "This option is for full Nginx vhost data and MariaDB MySQL data transfers from"
    echo "this server to a new Centmin Mod based server. There are specific requirements"
    echo "for this to work:"
    echo
    echo "1. The destination server must already have Centmin Mod LEMP stack installed"
    echo "2. The destination server and this server's MairaDB MySQL server versions must"
    echo "   be the same major version at least. i.e. MariaDB 10.3 or both"
    echo
    read -ep "Do you want to continue [y/n]: " datamanager_cont
    if [[ "$datamanager_cont" = [yY] ]]; then
      echo "place holder"
    else
      echo "aborting ..."
      exit
    fi


  } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_${datamenu_opt}_datamanger_menu.log"
;;
4)
  {
    check_for_datatools
    echo
    echo "Backup Nginx Vhosts Data + MariaBackup MySQL Backups"
    echo
    echo "Description:"
    echo "This option is for full Nginx vhost data and MariaDB MySQL backups using"
    echo "tar + zstd compression is optional. If you choose not to use tar + zstd"
    echo "the local backup directories will have uncompressed data backups."
    echo
    read -ep "Do you want to continue [y/n]: " datamanager_cont
    if [[ "$datamanager_cont" = [yY] ]]; then
      read -ep "Do you want tar + zstd compress backup [y/n]: " comp_backup_opt
      if [[ "$comp_backup_opt" = [yY] ]]; then
        "$SCRIPT_DIR/datamanagement/backups.sh" backup-all-mariabackup comp
      else
        "$SCRIPT_DIR/datamanagement/backups.sh" backup-all-mariabackup
      fi
    else
      echo "aborting ..."
      exit
    fi


  } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_${datamenu_opt}_datamanger_menu.log"
;;
5)
  {
    check_for_datatools
    echo
    echo "Backup Nginx Vhosts Data Only (no MariaDB MySQL backups)"
    echo
    echo "Description:"
    echo "This option is for full Nginx vhost data only backups (no MariaDB data backups)"
    echo "using tar + zstd compression is optional. If you choose not to use tar + zstd"
    echo "the local backup directories will have uncompressed data backups."
    echo
    read -ep "Do you want to continue [y/n]: " datamanager_cont
    if [[ "$datamanager_cont" = [yY] ]]; then
      read -ep "Do you want tar + zstd compress backup [y/n]: " comp_backup_opt
      if [[ "$comp_backup_opt" = [yY] ]]; then
        "$SCRIPT_DIR/datamanagement/backups.sh" backup-files comp
      else
        "$SCRIPT_DIR/datamanagement/backups.sh" backup-files
      fi
    else
      echo "aborting ..."
      exit
    fi


  } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_${datamenu_opt}_datamanger_menu.log"
;;
6)
  {
    check_for_datatools
    echo
    echo "Backup MariaDB MySQL With MariaBackup Only (no Vhosts Data backups)"
    echo
    echo "Description:"
    echo "This option is for MariaDB MySQL database backups only (no Nginx vhost data)"
    echo "using MariaBackup tool and not mysqldump for all MySQL databases backups"
    echo "+ mysql system database backups which contain your MySQL users' permissions"
    echo "MariaBackup backup directory can either be uncompressed or tar + zstd compressed"
    echo
    read -ep "Do you want to continue [y/n]: " datamanager_cont
    if [[ "$datamanager_cont" = [yY] ]]; then
      read -ep "Do you want tar + zstd compress backup [y/n]: " comp_backup_opt
      if [[ "$comp_backup_opt" = [yY] ]]; then
        "$SCRIPT_DIR/datamanagement/backups.sh" backup-mariabackup comp
      else
        "$SCRIPT_DIR/datamanagement/backups.sh" backup-mariabackup
      fi
    else
      echo "aborting ..."
      exit
    fi


  } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_${datamenu_opt}_datamanger_menu.log"
;;
7)
  {
    check_for_datatools
    echo
    echo "Backup MariaDB MySQL With mysqldump only (no Vhosts Data backups)"
    echo
    echo "Description:"
    echo "This option is for MariaDB MySQL database backups only using mysqldump native"
    echo "client but uses faster --tab delimited backup option which backups up each MySQL"
    echo "database table as separate .sql schema structure files + .txt data files rather"
    echo "than as a single .sql database backup file which contains both schema + data."
    echo "The .sql database schema only table files are never compressed while .txt data"
    echo "files can optionally be zstd compressed or left uncompressed."
    echo
    echo "The routine generates a restore.sh script in the destination backup directory"
    echo "The restore.sh can be run to restore each database or all databases on a new"
    echo "server. If the database name already exists on the server, it will restore to"
    echo "a new database name with suffix added _restorecopy_datetimestamp to prevent"
    echo "overwriting the existing database name."
    echo
    read -ep "Do you want to continue [y/n]: " datamanager_cont
    if [[ "$datamanager_cont" = [yY] ]]; then
      "$SCRIPT_DIR/datamanagement/backups.sh" backup-all
    else
      echo "aborting ..."
      exit
    fi


  } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_${datamenu_opt}_datamanger_menu.log"
;;
8)
  {
    check_for_datatools
    echo
    echo "Transfer Directory Data To Remote Server Via SSH"
    echo
    echo "Description:"
    echo "Option allows you to specify a full path to directory name for data transfer"
    echo "to a remote server via SSH at speeds near network and disk line rates using"
    echo "either netcat (nc) or socat compressed tunnel using zstd fast compression levels"
    echo
    read -ep "Do you want to continue [y/n]: " datamanager_cont
    if [[ "$datamanager_cont" = [yY] ]]; then
      echo "place holder"
    else
      echo "aborting ..."
      exit
    fi


  } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_${datamenu_opt}_datamanger_menu.log"
;;
9)
  {
    check_for_datatools
    echo
    echo "Transfer Directory Data To S3 Compatible Storage"
    echo
    echo "Description:"
    echo "Option allows you to specify a full path to directory name for data transfer"
    echo "to a S3 compatible storage provider like Amazon S3, Cloudflare R2, Backblaze"
    echo "DigitalOcean, Vultr, Linode"
    echo
    read -ep "Do you want to continue [y/n]: " datamanager_cont
    if [[ "$datamanager_cont" = [yY] ]]; then
      echo "place holder"
    else
      echo "aborting ..."
      exit
    fi


  } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_${datamenu_opt}_datamanger_menu.log"
;;
10)
  {
    check_for_datatools
    echo
    echo "Transfer Files To S3 Compatible Storage"
    echo
    echo "Description:"
    echo "Option allows you to specify a specific filename(s) for data transfer"
    echo "to a S3 compatible storage provider like Amazon S3, Cloudflare R2, Backblaze"
    echo "DigitalOcean, Vultr, Linode"
    echo
    read -ep "Do you want to continue [y/n]: " datamanager_cont
    if [[ "$datamanager_cont" = [yY] ]]; then
      echo "place holder"
    else
      echo "aborting ..."
      exit
    fi


  } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_${datamenu_opt}_datamanger_menu.log"
;;
11)
  {
    check_for_datatools
    echo
    echo "Download S3 Compatible Stored Data To Server"
    echo
    echo "Description:"
    echo "This option allows you to download your S3 stored data to this server"
    echo
    read -ep "Do you want to continue [y/n]: " datamanager_cont
    if [[ "$datamanager_cont" = [yY] ]]; then
      echo "place holder"
    else
      echo "aborting ..."
      exit
    fi


  } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_${datamenu_opt}_datamanger_menu.log"
;;
12)
  {
    check_for_datatools
    echo
    echo "S3 To S3 Compatible Storage Transfers"
    echo
    echo "Description:"
    echo "This option allows you to transfer data between two S3 compatible storage"
    echo
    read -ep "Do you want to continue [y/n]: " datamanager_cont
    if [[ "$datamanager_cont" = [yY] ]]; then
      echo "place holder"
    else
      echo "aborting ..."
      exit
    fi


  } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_${datamenu_opt}_datamanger_menu.log"
;;
13)
  {
    check_for_datatools
    echo
    echo "List S3 Storage Buckets"
    echo
    echo "Description:"
    echo "This option will list all your S3 storage buckets registered with the AWS CLI tool"
    echo
    read -ep "Do you want to continue [y/n]: " datamanager_cont
    if [[ "$datamanager_cont" = [yY] ]]; then
      echo "place holder"
    else
      echo "aborting ..."
      exit
    fi


  } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_${datamenu_opt}_datamanger_menu.log"
;;
14)
  break
;;
esac
done
}
