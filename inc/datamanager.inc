#######################################################################################
# Data Management Routines
#######################################################################################
AWSCLI_BIN='/usr/local/bin/aws'
DATAM_SOURCE_IP=$(curl -4s https://geoip.centminmod.com/v4 | jq -r '.ip')
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
  if [ ! -f "${AWSCLI_BIN}" ]; then
    echo
    echo "Installing AWS CLI command line tool ${AWSCLI_BIN}"
    "$SCRIPT_DIR/datamanagement/awscli-get.sh" install >/dev/null 2>&1 
  elif [ -f "${AWSCLI_BIN}" ]; then
    echo
    echo "Updating AWS CLI command line tool ${AWSCLI_BIN}"
    "$SCRIPT_DIR/datamanagement/awscli-get.sh" update >/dev/null 2>&1
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
  cecho "---------------------------------------------------------------------" $boldyellow
  cecho "      Manage AWS CLI S3 Profile Credentials        " $boldgreen
  cecho "---------------------------------------------------------------------" $boldyellow
  cecho "      Partially uses awscli-get.sh https://awscli-get.centminmod.com/" $boldgreen
  cecho "---------------------------------------------------------------------" $boldyellow
  cecho "1).   List Registered AWS CLI S3 Profiles" $boldgreen
  cecho "2).   List AWS CLI S3 Profile Configuration" $boldgreen
  cecho "3).   Create New AWS CLI S3 Profile" $boldgreen
  cecho "4).   Edit Existing AWS CLI S3 Profile" $boldgreen
  cecho "5).   Delete Existing AWS CLI S3 Profile" $boldgreen
  cecho "6).   Export Existing AWS CLI S3 Profile" $boldgreen
  cecho "7).   Backup All Existing AWS CLI S3 Profiles" $boldgreen
  cecho "8).   Back to Main menu" $boldgreen
  cecho "---------------------------------------------------------------------" $boldyellow

  read -ep "Enter option [ 1 - 8 ] " awscli_profiles_menu_opt
  cecho "---------------------------------------------------------------------" $boldyellow

#########################################################

case "$awscli_profiles_menu_opt" in
1)
  {
    check_for_awscli_get
    echo
    echo "List Registered AWS CLI S3 Profiles"
    echo
    ${AWSCLI_BIN} configure list-profiles

  } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_${awscli_profiles_menu_opt}_awscli_profiles_menu.log"
;;
2)
  {
    check_for_awscli_get
    echo
    echo "List AWS CLI S3 Profile Configuration"
    echo

    read -ep "Do you want to continue [y/n]: " aws_cli_profile_listconfig_cont
    if [[ "$aws_cli_profile_listconfig_cont" != [yY] ]]; then
      echo
      echo "exiting ..."
      exit
    fi

    while true; do
      read -ep "Enter AWS CLI S3 Profile Name For Configuration Overview: " input_profile_name
    
      echo
      echo "You entered AWS CLI S3 Profile Name = ${input_profile_name}"
      echo
      read -ep "Are these details correct? [y/n/exit]: " input_correct
    
      if [[ "$input_correct" =~ ^[yY]$ ]]; then
        ${AWSCLI_BIN} configure list --profile "${input_profile_name}"
        break
      elif [[ "$input_correct" == "exit" ]]; then
        echo
        echo "Exiting..."
        break
      else
        echo
        echo "Please re-enter your details."
        echo
      fi
    done

  } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_${awscli_profiles_menu_opt}_awscli_profiles_menu.log"
;;
3)
  {
    check_for_awscli_get
    echo
    echo "Create New AWS CLI S3 Profile"
    echo
    echo "To setup a new AWS CLI S3 Profile, you need to have signed up with a"
    echo "S3 storage provider like Amazon AWS S3, Backblaze, Cloudflare R2"
    echo "Linode, Vultr or DigitalOcean S3 object storage provider and obtained"
    echo "your S3 credentials for Access key ID and Secret Key and your endpoint"
    echo "url where applicable i.e. Cloudflare R2, Linode, DigitalOcean, Backblaze"
    echo "all have a their own specific endpoint url to use with their S3 storage."
    echo
    echo "The following questions and your input will be used to setup a new AWS"
    echo "CLI S3 profile."
    echo

    read -ep "Do you want to continue [y/n]: " aws_cli_profile_create_cont
    if [[ "$aws_cli_profile_create_cont" != [yY] ]]; then
      echo
      echo "exiting ..."
      exit
    fi

    while true; do
      read -ep "Your S3 Storage Provider i.e. aws, cloudflare, digitalocean, backblaze etc : " input_storage_provider
      read -ep "Desired AWS CLI S3 Profile Name: " input_profile_name
      read -ep "Endpoint url: i.e. for Cloudflare YOUR_CF_ACCOUNT_ID.r2.cloudflarestorage.com: " input_endpoint
      read -ep "Enter AWS_ACCESS_KEY_ID: " input_access_key
      read -ep "Enter AWS_SECRET_ACCESS_KEY: " input_secret_key
      read -ep "Enter AWS_DEFAULT_REGION for Cloudflare use r2: " input_default_region
      read -ep "Enter AWS_DEFAULT_OUTPUT: " input_default_output
    
      echo
      echo "You entered S3 Storage Provide=${input_storage_provider}"
      echo "You entered AWS CLI S3 Profile Name=${input_profile_name}"
      echo "Your entered endpoint url=${input_endpoint}"
      echo "You enter AWS_ACCESS_KEY_ID=${input_access_key}"
      echo "You enter AWS_SECRET_ACCESS_KEY=${input_secret_key}"
      echo "You enter AWS_DEFAULT_REGION=${input_default_region}"
      echo "You enter AWS_DEFAULT_OUTPUT=${input_default_output}"
      echo
      read -ep "Are these details correct? [y/n/exit]: " input_correct
    
      if [[ "$input_correct" =~ ^[yY]$ ]]; then
        export AWS_ACCESS_KEY_ID="${input_access_key}"
        export AWS_SECRET_ACCESS_KEY="${input_secret_key}"
        export AWS_DEFAULT_REGION="${input_default_region}"
        export AWS_DEFAULT_OUTPUT="${input_default_output}"

        # setup AWS CLI profile
        $SCRIPT_DIR/datamanagement/awscli-get.sh install "${input_profile_name}"
        break
      elif [[ "$input_correct" == "exit" ]]; then
        echo
        echo "Exiting..."
        break
      else
        echo
        echo "Please re-enter your details."
        echo
      fi
    done


  } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_${awscli_profiles_menu_opt}_awscli_profiles_menu.log"
;;
4)
  {
    check_for_awscli_get
    echo
    echo "Edit Existing AWS CLI S3 Profile"
    echo

    read -ep "Enter AWS CLI S3 Profile Name to Edit: " edit_profile_name
    echo "Fetching current configuration for profile ${edit_profile_name} ..."
    
    CONFIG_KEYS=("s3.max_concurrent_requests" "s3.multipart_threshold" "s3.multipart_chunksize" "s3.max_bandwidth" "s3.addressing_style" "s3.use_accelerate_endpoint" "s3.disable_ssl" "s3.verify_ssl" "region")

    for key in "${CONFIG_KEYS[@]}"; do
      value=$(${AWSCLI_BIN} configure get ${key} --profile "${edit_profile_name}")
      echo "   ${key}: ${value}"
    done

    while true; do
      echo
      echo "Select the configuration option you want to edit:"
      echo "1)  s3.max_concurrent_requests"
      echo "2)  s3.multipart_threshold"
      echo "3)  s3.multipart_chunksize"
      echo "4)  s3.max_bandwidth"
      echo "5)  s3.addressing_style"
      echo "6)  s3.use_accelerate_endpoint (AWS S3 only)"
      echo "7)  s3.disable_ssl (AWS S3 only)"
      echo "8)  s3.verify_ssl (AWS S3 only)"
      echo "9)  region"
      echo "10) exit"
    
      read -ep "Enter the number of the option you want to edit [ 1 - 10 ]: " selected_option
    
      if [[ "$selected_option" -eq 10 ]]; then
        break
      fi
    
      selected_key=${CONFIG_KEYS[selected_option - 1]}
      echo
      read -ep "Enter the new value for ${selected_key}: " new_value
    
      echo "You have chosen to update ${selected_key} to ${new_value}"
      read -ep "Is this the correct update value? [y/n/exit]: " input_correct
    
      if [[ "$input_correct" =~ ^[yY]$ ]]; then
        echo "${AWSCLI_BIN} configure set ${selected_key} \"${new_value}\" --profile \"${edit_profile_name}\""
        ${AWSCLI_BIN} configure set ${selected_key} "${new_value}" --profile "${edit_profile_name}"
        echo "Updated ${selected_key} to ${new_value} for profile ${edit_profile_name}"
        echo
        echo "Check new value for ${selected_key} = ${new_value} for profile ${edit_profile_name}"
        echo "${AWSCLI_BIN} configure get ${selected_key} --profile \"${edit_profile_name}\""
        ${AWSCLI_BIN} configure get ${selected_key} --profile "${edit_profile_name}"
        break
      elif [[ "$input_correct" == "exit" ]]; then
        echo
        echo "Exiting..."
        break
      else
        echo
        echo "Please re-enter your details."
      fi
    done
    
  } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_${awscli_profiles_menu_opt}_awscli_profiles_menu.log"
;;
5)
  {
    check_for_awscli_get
    echo
    echo "Delete Existing AWS CLI S3 Profile"
        echo

    read -ep "Do you want to continue [y/n]: " aws_cli_profile_delete_cont
    if [[ "$aws_cli_profile_delete_cont" != [yY] ]]; then
      echo
      echo "exiting ..."
      exit
    fi

    while true; do
      read -ep "Enter AWS CLI S3 Profile Name To Delete: " input_profile_name
    
      echo
      echo "You entered AWS CLI S3 Profile Name = ${input_profile_name}"
      echo
      read -ep "Are these details correct? [y/n/exit]: " input_correct
    
      if [[ "$input_correct" =~ ^[yY]$ ]]; then
        # Remove the profile from the configuration and credentials files
        aws configure --profile "${input_profile_name}" list  # Ensure the profile exists
        if [ $? -eq 0 ]; then
          CONFIG_FILE="${HOME}/.aws/config"
          CREDENTIALS_FILE="${HOME}/.aws/credentials"
          timestamp=$(date +%Y%m%d_%H%M%S)
          sed -i.${timestamp}.bak "/\[profile ${input_profile_name}\]/,/^$/d" "${CONFIG_FILE}"
          sed -i.${timestamp}.bak "/\[${input_profile_name}\]/,/^$/d" "${CREDENTIALS_FILE}"
          echo "AWS CLI S3 Profile '${input_profile_name}' deleted."
        else
          echo "Profile '${input_profile_name}' not found."
        fi
        break
      elif [[ "$input_correct" == "exit" ]]; then
        echo
        echo "Exiting..."
        break
      else
        echo
        echo "Please re-enter your details."
        echo
      fi
    done

  } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_${awscli_profiles_menu_opt}_awscli_profiles_menu.log"
;;
6)
  {
    check_for_awscli_get
    echo
    echo "Export Existing AWS CLI S3 Profile"
        echo

    read -ep "Do you want to continue [y/n]: " aws_cli_profile_export_cont
    if [[ "$aws_cli_profile_export_cont" != [yY] ]]; then
      echo
      echo "exiting ..."
      exit
    fi

    while true; do
      read -ep "Enter AWS CLI S3 Profile Name to Export: " input_profile_name
    
      echo
      echo "You entered AWS CLI S3 Profile Name = ${input_profile_name}"
      echo
      read -ep "Are these details correct? [y/n/exit]: " input_correct
    
      if [[ "$input_correct" =~ ^[yY]$ ]]; then
        timestamp=$(date +%Y%m%d_%H%M%S)
        mkdir -p /etc/centminmod/awscli/export-profiles

        awk -v profile="profile ${input_profile_name}" '
          $0 ~ /^\[profile .*\]$/ {flag=0}
          $0 ~ "^\\[" profile "\\]" {flag=1}
          flag {print}
        ' "${HOME}/.aws/config" > "/etc/centminmod/awscli/export-profiles/export-profile-${input_profile_name}-config-${timestamp}.conf"
      
        sed -n -e "/^\[${input_profile_name}\]/,/^\[/p" "${HOME}/.aws/credentials" > "/etc/centminmod/awscli/export-profiles/export-profile-${input_profile_name}-credentials-${timestamp}.conf"

        echo "Exported AWS CLI S3 Profile ${input_profile_name} to "
        echo "/etc/centminmod/awscli/export-profiles/export-profile-${input_profile_name}-config-${timestamp}.conf"
        echo "/etc/centminmod/awscli/export-profiles/export-profile-${input_profile_name}-credentials-${timestamp}.conf"
        break
      elif [[ "$input_correct" == "exit" ]]; then
        echo
        echo "Exiting..."
        break
      else
        echo
        echo "Please re-enter your details."
        echo
      fi
    done

  } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_${awscli_profiles_menu_opt}_awscli_profiles_menu.log"
;;
7)
  {
    check_for_awscli_get
    echo
    echo "Backup All Existing AWS CLI S3 Profiles"
    echo

    read -ep "Do you want to continue [y/n]: " aws_cli_profiles_backup_cont
    if [[ "$aws_cli_profiles_backup_cont" != [yY] ]]; then
      echo
      echo "exiting ..."
      exit
    fi

    timestamp=$(date +%Y%m%d_%H%M%S)
    backup_directory="/etc/centminmod/awscli/backup-profiles"
    mkdir -p "${backup_directory}"

    cp "${HOME}/.aws/config" "${backup_directory}/config_backup-${timestamp}.conf"
    cp "${HOME}/.aws/credentials" "${backup_directory}/credentials_backup-${timestamp}.conf"

    echo "All AWS CLI S3 Profiles backed up to :"
    echo "${backup_directory}/config_backup-${timestamp}.conf"
    echo "${backup_directory}/credentials_backup-${timestamp}.conf"

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
  cecho "--------------------------------------------------------------" $boldyellow
  cecho "      Manage SSH Keys        " $boldgreen
  cecho "--------------------------------------------------------------" $boldyellow
  cecho "      Uses keygen.sh tool https://github.com/centminmod/keygen" $boldgreen
  cecho "--------------------------------------------------------------" $boldyellow
  cecho "1).   List Registered SSH Keys" $boldgreen
  cecho "2).   Register Existing SSH Keys" $boldgreen
  cecho "3).   Create New SSH Key For Remote Host" $boldgreen
  cecho "4).   Use Existing SSH Key For Remote Host" $boldgreen
  cecho "5).   Rotate Existing SSH Key For Remote Host" $boldgreen
  cecho "6).   Delete Existing SSH Key For Remote Host" $boldgreen
  cecho "7).   Export Existing SSH Key" $boldgreen
  cecho "8).   Backup All Existing SSH Keys" $boldgreen
  cecho "9).   Back to Main menu" $boldgreen
  cecho "--------------------------------------------------------------" $boldyellow

  read -ep "Enter option [ 1 - 9 ] " sshkeys_menu_opt
  cecho "--------------------------------------------------------------" $boldyellow

#########################################################

case "$sshkeys_menu_opt" in
1)
  {
    check_for_keygen
    files=$(find /root/.ssh/ -type f -name "my*.key")
    
    if [ -n "$files" ]; then
        echo "Found matching keygen.sh generated SSH private key files."
        echo
        for file in $files; do
            # ls -lAh $file
            echo -n "${file}.pub: "
            ssh-keygen -lf ${file}.pub
        done
    else
        echo "No matching keygen.sh generated SSH private key files found."
    fi
    echo
    echo "List Registered SSH Keys"
    echo
    if [ ! -n "$SSH_AGENT_PID" ]; then
      eval $(ssh-agent -s)
    fi
    ssh_keys_info=$(ssh-add -l 2>&1 | egrep -iv 'error|Not found')

    if [[ -z "$ssh_keys_info" ]]; then
      echo "No SSH keys registered with the system's ssh-agent."
    else
      while IFS= read -r line; do
        fingerprint=$(echo "$line" | awk '{print $2}')
        echo -n "$line "
        keyfile=$(grep -rl "$fingerprint" /root/.ssh 2>/dev/null)

        if [[ -n "$keyfile" ]]; then
          echo "- Key File: $keyfile"
        else
          echo "- Key File: Not found"
        fi
      done <<< "$ssh_keys_info"
    fi
  } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_${sshkeys_menu_opt}_sshkeys_menu.log"
;;
2)
  {
    check_for_keygen
    echo
    echo "Register Existing SSH Keys"
    echo

    read -ep "Enter the path to the existing SSH private key file: " private_key_path
    
    if [[ -f "$private_key_path" ]]; then
      # Check if ssh-agent is already running
      if [ -n "$SSH_AGENT_PID" ]; then
        echo "An ssh-agent session is already running with PID: $SSH_AGENT_PID"
      else
        # Start a new ssh-agent session if not already running
        eval "$(ssh-agent -s)"
      fi
      
      # Check if the key is already added
      if ssh-add -L | grep -q "$private_key_path"; then
        echo "The specified SSH private key is already added."
      else
        ssh-add "$private_key_path"
        echo "SSH private key registered with the system's ssh-agent."
      fi
    else
      echo "Error: The specified private key file does not exist."
    fi
  } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_${sshkeys_menu_opt}_sshkeys_menu.log"
;;
3)
  {
    check_for_keygen
    echo
    echo "Create New SSH Key For Remote Host"
    echo
    
    while true; do
      # Prompt user for required input
      read -ep "Key type (rsa, ecdsa, ed25519): " -i "ed25519" keytype
      read -ep "Remote IP address: " remoteip
      read -ep "Remote SSH port: " remoteport
      read -ep "Remote SSH username: " remoteuser
      read -ep "Key comment (unique identifier): " keycomment
      read -ep "Remote SSH password (optional, leave empty for manual password entry): " remotessh_password

      echo "Please confirm the following information:"
      echo "Key type: $keytype"
      echo "Remote IP address: $remoteip"
      echo "Remote SSH port: $remoteport"
      echo "Remote SSH username: $remoteuser"
      echo "Key comment: $keycomment"
      echo "Remote SSH password: ${remotessh_password:-Not provided}"

      read -ep "Is the above information correct? [y/n/exit]: " confirm_input

      case "$confirm_input" in
        [Yy]* )
          if [ -z "$remotessh_password" ]; then
            # Call keygen.sh without remotessh_password
            "$SCRIPT_DIR/datamanagement/keygen.sh" gen "$keytype" "$remoteip" "$remoteport" "$remoteuser" "$keycomment"
          else
            # Call keygen.sh with remotessh_password
            "$SCRIPT_DIR/datamanagement/keygen.sh" gen "$keytype" "$remoteip" "$remoteport" "$remoteuser" "$keycomment" "$remotessh_password"
          fi
          break
          ;;
        [Nn]* )
          echo "Please enter the correct information."
          ;;
        [Ee][Xx][Ii][Tt] )
          echo "Exiting the menu."
          break
          ;;
        * )
          echo "Please answer with 'y', 'n', or 'exit'."
          ;;
      esac
    done

  } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_${sshkeys_menu_opt}_sshkeys_menu.log"
;;
4)
  {
    check_for_keygen
    echo
    echo "Use Existing SSH Key For Remote Host"
    echo

    # List Registered SSH Keys
    ssh_keys_info=$(ssh-add -l 2>&1 | grep -v error)

    if [[ -z "$ssh_keys_info" ]]; then
      echo "No SSH keys registered with the system's ssh-agent."
    else
      while IFS= read -r line; do
        fingerprint=$(echo "$line" | awk '{print $2}')
        echo -n "$line "
        keyfile=$(grep -rl "$fingerprint" /root/.ssh 2>/dev/null)

        if [[ -n "$keyfile" ]]; then
          echo "- Key File: $keyfile"
        else
          echo "- Key File: Not found"
        fi
      done <<< "$ssh_keys_info"
    fi

    while true; do
      # Prompt user for required input
      read -ep "Key name (without file extension): " key_name
      read -ep "Remote IP address: " remoteip
      read -ep "Remote SSH port: " remoteport
      read -ep "Remote SSH username: " remoteuser
      read -ep "Remote SSH password (optional, leave empty for manual password entry): " remotessh_password

      echo "Please confirm the following information:"
      echo "Key name: $key_name"
      echo "Remote IP address: $remoteip"
      echo "Remote SSH port: $remoteport"
      echo "Remote SSH username: $remoteuser"
      echo "Remote SSH password: ${remotessh_password:-N/A}"

      read -ep "Is the above information correct? [y/n/exit]: " confirm_input

      case "$confirm_input" in
        [Yy]* )
          if [[ -z "$remotessh_password" ]]; then
            ssh-copy-id -i "/root/.ssh/${key_name}" "${remoteuser}@${remoteip}" -p "${remoteport}"
          else
            sshpass -p "$remotessh_password" ssh-copy-id -o StrictHostKeyChecking=no -i "/root/.ssh/${key_name}" "${remoteuser}@${remoteip}" -p "${remoteport}"
          fi
          echo "SSH key '$key_name' copied to the remote host."
          break
          ;;
        [Nn]* )
          echo "Please enter the correct information."
          ;;
        [Ee][Xx][Ii][Tt] )
          echo "Exiting the menu."
          break
          ;;
        * )
          echo "Please answer with 'y', 'n', or 'exit'."
          ;;
      esac
    done
  } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_${sshkeys_menu_opt}_sshkeys_menu.log"
;;
5)
  {
    check_for_keygen
    echo
    echo "Rotate Existing SSH Key For Remote Host"
    echo
    
    while true; do
      # Prompt user for required input
      read -ep "Key type (rsa, ecdsa, ed25519): " -i "ed25519" keytype
      read -ep "Remote IP address: " remoteip
      read -ep "Remote SSH port: " remoteport
      read -ep "Remote SSH username: " remoteuser
      read -ep "Old Key comment (unique identifier): " old_keycomment
      read -ep "New Key comment (unique identifier): " new_keycomment
      read -ep "Remote SSH password (optional, leave empty for manual password entry): " remotessh_password

      echo "Please confirm the following information:"
      echo "Key type: $keytype"
      echo "Remote IP address: $remoteip"
      echo "Remote SSH port: $remoteport"
      echo "Remote SSH username: $remoteuser"
      echo "Old Key comment: $old_keycomment"
      echo "New Key comment: $new_keycomment"
      echo "Remote SSH password: ${remotessh_password:-Not provided}"

      read -ep "Is the above information correct? [y/n/exit]: " confirm_input

      case "$confirm_input" in
        [Yy]* )
          if [ -z "$remotessh_password" ]; then
            # Call keygen.sh without remotessh_password
            "$SCRIPT_DIR/datamanagement/keygen.sh" rotate "$keytype" "$remoteip" "$remoteport" "$remoteuser" "$old_keycomment" "$new_keycomment"
          else
            # Call keygen.sh with remotessh_password
            "$SCRIPT_DIR/datamanagement/keygen.sh" rotate "$keytype" "$remoteip" "$remoteport" "$remoteuser" "$old_keycomment" "$new_keycomment" "$remotessh_password"
          fi
          echo "SSH key rotation completed."
          break
          ;;
        [Nn]* )
          echo "Please enter the correct information."
          ;;
        [Ee][Xx][Ii][Tt] )
          echo "Exiting the menu."
          break
          ;;
        * )
          echo "Please answer with 'y', 'n', or 'exit'."
          ;;
      esac
    done

  } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_${sshkeys_menu_opt}_sshkeys_menu.log"
;;
6)
  {
    check_for_keygen
    echo
    echo "Delete Existing SSH Key For Remote Host"
    echo

    while true; do
      # Prompt user for required input
      read -ep "Remote IP address: " remoteip
      read -ep "Remote SSH port: " remoteport
      read -ep "Remote SSH username: " remoteuser
      read -ep "Key comment (unique identifier): " keycomment
      read -ep "Private key path (optional, leave empty if not using a specific key): " private_key_path
      read -ep "Remote SSH password (optional, leave empty for manual password entry): " remotessh_password

      echo "Please confirm the following information:"
      echo "Remote IP address: $remoteip"
      echo "Remote SSH port: $remoteport"
      echo "Remote SSH username: $remoteuser"
      echo "Key comment: $keycomment"
      echo "Private key path: ${private_key_path:-Not provided}"
      echo "Remote SSH password: ${remotessh_password:-Not provided}"

      read -ep "Is the above information correct? [y/n/exit]: " confirm_input

      case "$confirm_input" in
        [Yy]* )
          # Delete the SSH key from the remote host
          if [ -z "$private_key_path" ]; then
            ssh_cmd="ssh"
          else
            ssh_cmd="ssh -i $private_key_path"
          fi

          if [ -z "$remotessh_password" ]; then
            # Use the provided ssh command without the remotessh_password
            $ssh_cmd "${remoteuser}@${remoteip}" -p "${remoteport}" "sed -i '/${keycomment}/d' ~/.ssh/authorized_keys"
          else
            # Use the provided ssh command with the remotessh_password
            sshpass -p "$remotessh_password" $ssh_cmd "${remoteuser}@${remoteip}" -p "${remoteport}" "sed -i '/${keycomment}/d' ~/.ssh/authorized_keys"
          fi

          echo "SSH key deletion completed."
          break
          ;;
        [Nn]* )
          echo "Please enter the correct information."
          ;;
        [Ee][Xx][Ii][Tt] )
          echo "Exiting the menu."
          break
          ;;
        * )
          echo "Please answer with 'y', 'n', or 'exit'."
          ;;
      esac
    done

  } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_${sshkeys_menu_opt}_sshkeys_menu.log"
;;
7)
  {
    check_for_keygen
    echo
    echo "Export (Backup) Existing SSH Key"
    echo

    while true; do
      # List Registered SSH Keys
      ssh_keys_info=$(ssh-add -l 2>&1 | grep -v error)

      if [[ -z "$ssh_keys_info" ]]; then
        echo "No SSH keys registered with the system's ssh-agent."
      else
        while IFS= read -r line; do
          fingerprint=$(echo "$line" | awk '{print $2}')
          echo -n "$line "
          keyfile=$(grep -rl "$fingerprint" /root/.ssh 2>/dev/null)

          if [[ -n "$keyfile" ]]; then
            echo "- Key File: $keyfile"
          else
            echo "- Key File: Not found"
          fi
        done <<< "$ssh_keys_info"
      fi

      # Prompt user for required input
      read -ep "Key name (without file extension): " key_name
      read -ep "Destination path for key backup: " backup_path

      echo "Please confirm the following information:"
      echo "Key name: $key_name"
      echo "Destination path for key backup: $backup_path"

      read -ep "Is the above information correct? [y/n/exit]: " confirm_input

      case "$confirm_input" in
        [Yy]* )
          # Check if key files exist
          if [[ -f "/root/.ssh/${key_name}" && -f "/root/.ssh/${key_name}.pub" ]]; then
            # Create backup directory if it doesn't exist
            mkdir -p "$backup_path"

            # Backup the specified key files
            cp "/root/.ssh/${key_name}" "${backup_path}/${key_name}"
            cp "/root/.ssh/${key_name}.pub" "${backup_path}/${key_name}.pub"

            echo "SSH key '$key_name' backed up successfully to $backup_path."
          else
            echo "Error: SSH key '$key_name' not found in /root/.ssh/"
          fi
          break
          ;;
        [Nn]* )
          echo "Please enter the correct information."
          ;;
        [Ee][Xx][Ii][Tt] )
          echo "Exiting the menu."
          break
          ;;
        * )
          echo "Please answer with 'y', 'n', or 'exit'."
          ;;
      esac
    done

  } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_${sshkeys_menu_opt}_sshkeys_menu.log"
;;
8)
  {
    check_for_keygen
    echo
    echo "Backup All Existing SSH Keys"
    echo

    while true; do
      # Prompt user for required input
      read -ep "Destination path for SSH keys backup: " backup_path

      echo "Please confirm the following information:"
      echo "Destination path for SSH keys backup: $backup_path"

      read -ep "Is the above information correct? [y/n/exit]: " confirm_input

      case "$confirm_input" in
        [Yy]* )
          # Create backup directory if it doesn't exist
          mkdir -p "$backup_path"

          # Find all private and public key files in /root/.ssh/
          ssh_key_files=$(find /root/.ssh -type f \( -iname "*.pub" -o ! -iname "known_hosts" -o ! -iname "authorized_keys" -o ! -iname "config" \))

          # Backup all SSH keys
          for key_file in $ssh_key_files; do
            cp "$key_file" "${backup_path}/"
          done

          echo "All SSH keys backed up successfully to $backup_path."
          break
          ;;
        [Nn]* )
          echo "Please enter the correct information."
          ;;
        [Ee][Xx][Ii][Tt] )
          echo "Exiting the menu."
          break
          ;;
        * )
          echo "Please answer with 'y', 'n', or 'exit'."
          ;;
      esac
    done

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
  cecho "      Centmin Mod Data Management        " $boldgreen
  cecho "--------------------------------------------------------" $boldyellow
  cecho "      Docs: https://centminmod.com/menu21-${branchname}" $boldgreen
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
      echo
      echo "Choose the backup method:"
      echo "1) Backup Nginx Vhosts Data + MariaBackup MySQL Backups"
      echo "2) Backup Nginx Vhosts Data + Backup MariaDB MySQL With mysqldump"
      echo
      read -ep "Enter the backup method (1 or 2): " backup_method
      
      case "$backup_method" in
        1)
          # Call the code for 4) Backup Nginx Vhosts Data + MariaBackup MySQL Backups
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
              # echo
              # echo "Compressed has full backup - vhost files + MySQL backup including non-vhost files"
              # echo "Non-compressed has vhost files + MySQL backup only."
              # echo
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
          } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_backupmethod-${backup_method}_datamanger_menu.log"
            if [ -f "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_backupmethod-${backup_method}_datamanger_menu.log" ]; then
              script_output=$(cat "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_backupmethod-${backup_method}_datamanger_menu.log")
              # transfer_backup_dir=$(echo "$script_output" | grep 'Backup Log saved: ' | awk '{print $10}' | xargs dirname)
              transfer_backup_dir=$(echo "$script_output" | grep 'Backup Log saved:' | sed -E 's/.*Backup Log saved: ([^ ]+).*/\1/' | xargs dirname)
              # Print the extracted information
              echo
              if [ -d "$transfer_backup_dir" ]; then
                echo "Backup Directory: $transfer_backup_dir"
              else
                echo "$transfer_backup_dir directory not found"
              fi
              # routine to transfer the derived backup directory names to a remote server via SSH
              # using $SCRIPT_DIR/datamanagement/tunnel-transfers.sh tool
              echo
              read -ep "Do you want to transfer the backup directory to remote server via SSH? [y/n/exit]: " transfer_backup_opt
              if [[ "$transfer_backup_opt" = [yY] ]]; then
                # transfer backups
                echo
                echo "Transfer $transfer_backup_dir Directory Data To Remote Server Via SSH"
                echo
                echo "Description:"
                echo "Option allows you to specify a full path to directory name for data transfer"
                echo "to a remote server via SSH at speeds near network and disk line rates using"
                echo "either netcat (nc) or socat compressed tunnel using zstd fast compression levels"
                echo
                read -ep "Do you want to continue [y/n]: " datamanager_cont
                if [[ "$datamanager_cont" = [yY] ]]; then
                  while true; do
                    read -ep "Remote server SSH port (default: 22): " remote_port
                    remote_port=${remote_port:-22}
                    read -ep "Remote server SSH username (default: root): " remote_user
                    if [ "$remote_user" != "root" ]; then
                      echo "non-root user detected, will assume remote SSH username has sudo privileges"
                      echo
                      read -ep "continue with $remote_user with assumed sudo privileges [y/n] ?" assume_sudo_user
                      if [[ "$assume_sudo_user" != [yY] ]]; then
                        echo
                        echo "Enter a different username with sudo privileges or root user"
                        read -ep "Re-enter remote server SSH username: " remote_user
                      fi
                    fi
                    remote_user=${remote_user:-root}
                    read -ep "Remote server SSH hostname/IP address: " remote_server
                    read -ep "Tunnel method (nc or socat, default: nc): " tunnel_method
                    tunnel_method=${tunnel_method:-nc}
                    read -ep "Buffer size for socat (in bytes, e.g., 131072 for 128 KB): " buffer_size
                    buffer_size=${buffer_size:-131072}
                    read -ep "Listen port for nc or socat (default: 12345): " listen_port
                    listen_port=${listen_port:-12345}
                    read -e -i "$transfer_backup_dir" -p "Source backup directory: " source_directory
                    read -ep "Remote (destination) backup directory i.e. /home/remotebackup : " remote_directory
                    remote_directory=${remote_directory:-/home/remotebackup}
                    read -ep "Path to the SSH private key i.e. /root/.ssh/my1.key : " private_key
                
                    echo
                    echo "Please confirm the entered information:"
                    echo "Remote server SSH port: $remote_port"
                    echo "Remote server SSH username: $remote_user"
                    echo "Remote server SSH hostname/IP address: $remote_server"
                    echo "Tunnel method: $tunnel_method"
                    echo "Buffer size: $buffer_size"
                    echo "Listen port: $listen_port"
                    echo "Source backup directory: $source_directory"
                    echo "Remote (destination) backup directory: $remote_directory"
                    echo "Path to the SSH private key: $private_key"
                    echo
                
                    read -ep "Is the information correct? [y/n]: " confirmation
                
                    if [[ "$confirmation" =~ ^[yY] ]]; then
                      break
                    else
                      echo
                      echo "Please re-enter the information."
                      echo
                    fi
                  done

                  # allow remote IP in source CSF Firewall
                  echo
                  echo "csf -a $remote_server remotebackup"
                  csf -a $remote_server remotebackup
                  echo

                  # connect to remote destination to run CSF Firewall allow
                  echo "ssh -i $private_key -p $remote_port $remote_user@$remote_server \"csf -a $DATAM_SOURCE_IP cmmbackupip\""
                  ssh -i $private_key -p $remote_port $remote_user@$remote_server "csf -a $DATAM_SOURCE_IP cmmbackupip"
                  echo
                
                  echo "$SCRIPT_DIR/datamanagement/tunnel-transfers.sh -p $remote_port -u $remote_user -h $remote_server -m $tunnel_method -b $buffer_size -l $listen_port -s $source_directory -r $remote_directory -k $private_key"
                  "$SCRIPT_DIR/datamanagement/tunnel-transfers.sh" -p "$remote_port" -u "$remote_user" -h "$remote_server" -m "$tunnel_method" -b "$buffer_size" -l "$listen_port" -s "$source_directory" -r "$remote_directory" -k "$private_key"
                else
                  echo "aborting ..."
                  exit
                fi
              elif [[ "$transfer_backup_opt" = 'exit' ]]; then
                # exit
                exit
              else
                # exit
                exit
              fi
            fi
        ;;
        2)
          # Call the code for 5) Backup Nginx Vhosts Data Only
          # Call the code for 7) Backup MariaDB MySQL With mysqldump only (no Vhosts Data backups)
          {
            check_for_datatools
            echo
            echo "Backup Nginx Vhosts Data Only (no MariaDB MySQL backups)"
            echo
            echo "Vhosts backup:"
            echo "This option is for full Nginx vhost data backups"
            echo "using tar + zstd compression is optional. If you choose not to use tar + zstd"
            echo "the local backup directories will have uncompressed data backups."
            echo
            echo "MySQL backup:"
            echo "For MariaDB MySQL database backups using mysqldump native client"
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
            # echo
            # echo "Compressed has full backup - vhost files + MySQL backup including non-vhost files"
            # echo "Non-compressed has vhost files + MySQL backup only."
            echo
            read -ep "Do you want tar + zstd compress backup [y/n] " comp_backup_opt
            if [[ "$comp_backup_opt" = [yY] ]]; then
              "$SCRIPT_DIR/datamanagement/backups.sh" backup-files comp
              "$SCRIPT_DIR/datamanagement/backups.sh" backup-all
            else
              "$SCRIPT_DIR/datamanagement/backups.sh" backup-files
              "$SCRIPT_DIR/datamanagement/backups.sh" backup-all
            fi
          } 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_backupmethod-${backup_method}_datamanger_menu.log"
            if [ -f "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_backupmethod-${backup_method}_datamanger_menu.log" ]; then
              # Save the output of the script to a variable
              script_output=$(cat "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_backupmethod-${backup_method}_datamanger_menu.log" | egrep 'log file generated: |Backup Log saved: |Backup saved to' | sed -e 's|Backup Log saved:|Vhost Backup Log saved:|' -e 's|Backup log file generated|MySQL Backup log file generated|g')
              
              # Extract the required information using grep and awk
              # transfer_vhost_backup_dir=$(echo "$script_output" | grep "Vhost Backup Log saved:" | awk '{print $11}' | xargs dirname)
              transfer_vhost_backup_dir=$(echo "$script_output" | grep 'Vhost Backup Log saved:' | sed -E 's/.*Vhost Backup Log saved: ([^ ]+).*/\1/' | xargs dirname)
              transfer_backup_saved_mysql=$(echo "$script_output" | grep "Backup saved to" | head -n 1 | awk '{print $10}')
              transfer_backup_saved_binlog=$(echo "$script_output" | grep "Backup saved to" | tail -n 1 | awk '{print $10}')
              
              # Print the extracted information
              echo
              if [ -d "$transfer_vhost_backup_dir" ]; then
                echo "Vhost Backup Directory: $transfer_vhost_backup_dir"
              # else
              #   echo "$transfer_vhost_backup_dir directory not found"
              fi
              if [ -d "$transfer_backup_saved_mysql" ]; then
                echo "MySQL Backup Directory: $transfer_backup_saved_mysql"
              # else
              #   echo "$transfer_backup_saved_mysql directory not found"
              fi
              if [ -d "$transfer_backup_saved_binlog" ]; then
                echo "Binlog Backup Directory: $transfer_backup_saved_binlog"
              fi
              # routine to transfer the derived backup directory names to a remote server via SSH
              # using $SCRIPT_DIR/datamanagement/tunnel-transfers.sh tool
              echo
              read -ep "Do you want to transfer the backup directories to remote server via SSH? [y/n/exit]: " transfer_backup_opt
              if [[ "$transfer_backup_opt" = [yY] ]]; then
                read -ep "Remote server SSH port (default: 22): " remote_port
                remote_port=${remote_port:-22}
                read -ep "Remote server SSH username (default: root): " remote_user
                if [ "$remote_user" != "root" ]; then
                  echo "non-root user detected, will assume remote SSH username has sudo privileges"
                  echo
                  read -ep "continue with $remote_user with assumed sudo privileges [y/n] ?" assume_sudo_user
                  if [[ "$assume_sudo_user" != [yY] ]]; then
                    echo
                    echo "Enter a different username with sudo privileges or root user"
                    read -ep "Re-enter remote server SSH username: " remote_user
                  fi
                fi
                remote_user=${remote_user:-root}
                read -ep "Remote server SSH hostname/IP address: " remote_server
                read -ep "Tunnel method (nc or socat, default: nc): " tunnel_method
                tunnel_method=${tunnel_method:-nc}
                read -ep "Buffer size for socat (in bytes, e.g., 131072 for 128 KB): " buffer_size
                buffer_size=${buffer_size:-131072}
                read -ep "Listen port for nc or socat (default: 12345): " listen_port
                listen_port=${listen_port:-12345}
                # read -e -i "$transfer_backup_dir" -p "Source backup directory: " source_directory
                read -ep "Remote (destination) backup directory i.e. /home/remotebackup : " remote_directory
                remote_directory=${remote_directory:-/home/remotebackup}
                read -ep "Path to the SSH private key i.e. /root/.ssh/my1.key : " private_key

                # An associative array containing the directories to transfer and their corresponding subdirectories
                declare -A directories_to_transfer=(
                    ["$transfer_vhost_backup_dir"]="${remote_directory}/vhosts"
                    ["$transfer_backup_saved_mysql"]="${remote_directory}/mysql"
                    ["$transfer_backup_saved_binlog"]="${remote_directory}/mysql-binlogs"
                )

                # Iterate over the directories and execute the transfer routine for each directory
                for transfer_backup_dir in "${!directories_to_transfer[@]}"; do
                  remote_subdir="${directories_to_transfer[$transfer_backup_dir]}"
                  # transfer backups
                  echo
                  echo "Transfer $transfer_backup_dir Directory Data To Remote Server Via SSH"
                  echo
                  read -ep "Do you want to continue [y/n]: " datamanager_cont
                  if [[ "$datamanager_cont" = [yY] ]]; then
                    # allow remote IP in source CSF Firewall
                    echo
                    echo "csf -a $remote_server remotebackup"
                    csf -a $remote_server remotebackup
                    echo
  
                    # connect to remote destination to run CSF Firewall allow
                    echo "ssh -i $private_key -p $remote_port $remote_user@$remote_server \"csf -a $DATAM_SOURCE_IP cmmbackupip\""
                    ssh -i $private_key -p $remote_port $remote_user@$remote_server "csf -a $DATAM_SOURCE_IP cmmbackupip"
                    echo

                    echo "$SCRIPT_DIR/datamanagement/tunnel-transfers.sh -p $remote_port -u $remote_user -h $remote_server -m $tunnel_method -b $buffer_size -l $listen_port -s $transfer_backup_dir -r $remote_subdir -k $private_key"
                    "$SCRIPT_DIR/datamanagement/tunnel-transfers.sh" -p "$remote_port" -u "$remote_user" -h "$remote_server" -m "$tunnel_method" -b "$buffer_size" -l "$listen_port" -s "$transfer_backup_dir" -r "$remote_subdir" -k "$private_key"
                  else
                    echo "aborting ..."
                    exit
                  fi
                done
              elif [[ "$transfer_backup_opt" = 'exit' ]]; then
                # exit
                exit
              else
                # exit
                exit
              fi
            fi
        ;;
        *)
          echo "Invalid option. Aborting migration."
          exit
        ;;
      esac
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
      # echo
      # echo "Compressed has full backup - vhost files + MySQL backup including non-vhost files"
      # echo "Non-compressed has vhost files + MySQL backup only."
      # echo
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
      # echo
      # echo "Compressed has full backup - vhost files including non-vhost files"
      # echo "Non-compressed has vhost files only."
      # echo
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
      while true; do
        read -ep "Remote server SSH port (default: 22): " remote_port
        remote_port=${remote_port:-22}
        read -ep "Remote server SSH username (default: root): " remote_user
        if [ "$remote_user" != "root" ]; then
          echo "non-root user detected, will assume remote SSH username has sudo privileges"
          echo
          read -ep "continue with $remote_user with assumed sudo privileges [y/n] ?" assume_sudo_user
          if [[ "$assume_sudo_user" != [yY] ]]; then
            echo
            echo "Enter a different username with sudo privileges or root user"
            read -ep "Re-enter remote server SSH username: " remote_user
          fi
        fi
        remote_user=${remote_user:-root}
        read -ep "Remote server SSH hostname/IP address: " remote_server
        read -ep "Tunnel method (nc or socat, default: nc): " tunnel_method
        tunnel_method=${tunnel_method:-nc}
        read -ep "Buffer size for socat (in bytes, e.g., 131072 for 128 KB): " buffer_size
        buffer_size=${buffer_size:-131072}
        read -ep "Listen port for nc or socat (default: 12345): " listen_port
        listen_port=${listen_port:-12345}
        read -ep "Source backup directory: " source_directory
        read -ep "Remote (destination) backup directory i.e. /home/remotebackup : " remote_directory
        remote_directory=${remote_directory:-/home/remotebackup}
        read -ep "Path to the SSH private key i.e. /root/.ssh/my1.key : " private_key
    
        echo
        echo "Please confirm the entered information:"
        echo "Remote server SSH port: $remote_port"
        echo "Remote server SSH username: $remote_user"
        echo "Remote server SSH hostname/IP address: $remote_server"
        echo "Tunnel method: $tunnel_method"
        echo "Buffer size: $buffer_size"
        echo "Listen port: $listen_port"
        echo "Source backup directory: $source_directory"
        echo "Remote (destination) backup directory: $remote_directory"
        echo "Path to the SSH private key: $private_key"
        echo
    
        read -ep "Is the information correct? [y/n]: " confirmation
    
        if [[ "$confirmation" =~ ^[yY] ]]; then
          break
        else
          echo
          echo "Please re-enter the information."
          echo
        fi
      done

      # allow remote IP in source CSF Firewall
      echo
      echo "csf -a $remote_server remotebackup"
      csf -a $remote_server remotebackup
      echo

      # connect to remote destination to run CSF Firewall allow
      echo "ssh -i $private_key -p $remote_port $remote_user@$remote_server \"csf -a $DATAM_SOURCE_IP cmmbackupip\""
      ssh -i $private_key -p $remote_port $remote_user@$remote_server "csf -a $DATAM_SOURCE_IP cmmbackupip"
      echo
    
      echo "$SCRIPT_DIR/datamanagement/tunnel-transfers.sh -p $remote_port -u $remote_user -h $remote_server -m $tunnel_method -b $buffer_size -l $listen_port -s $source_directory -r $remote_directory -k $private_key"
      "$SCRIPT_DIR/datamanagement/tunnel-transfers.sh" -p "$remote_port" -u "$remote_user" -h "$remote_server" -m "$tunnel_method" -b "$buffer_size" -l "$listen_port" -s "$source_directory" -r "$remote_directory" -k "$private_key"
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
      while true; do
        read -ep "AWS CLI profile name: " aws_profile
        read -ep "S3 bucket name: " s3_bucket
        read -ep "S3 endpoint URL: " s3_endpoint
        read -ep "Source directory path: " source_directory
    
        echo
        echo "Please confirm the entered information:"
        echo "AWS CLI profile name: $aws_profile"
        echo "S3 bucket name: $s3_bucket"
        echo "S3 endpoint URL: $s3_endpoint"
        echo "Source directory path: $source_directory"
        echo
    
        read -ep "Is the information correct? [y/n]: " confirmation
    
        if [[ "$confirmation" =~ ^[yY] ]]; then
          break
        else
          echo
          echo "Please re-enter the information."
          echo
        fi
      done
    
      echo "aws s3 sync --profile $aws_profile --endpoint-url $s3_endpoint $source_directory s3://$s3_bucket/"
      aws s3 sync --profile "$aws_profile" --endpoint-url "$s3_endpoint" "$source_directory" "s3://$s3_bucket/"
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
      while true; do
        read -ep "AWS CLI profile name: " aws_profile
        read -ep "S3 bucket name: " s3_bucket
        read -ep "S3 endpoint URL: " s3_endpoint
        read -ep "File path to transfer: " file_path

        read -ep "Do you want to add additional files to transfer? [y/n]: " additional_files
        if [[ "$additional_files" =~ ^[yY] ]]; then
          read -ep "How many additional files do you want to transfer? (Enter a number): " num_files
          for i in $(seq 1 $num_files); do
            read -ep "File path to transfer (file $i): " additional_file_path
            file_path="$file_path $additional_file_path"
          done
        fi

        echo
        echo "Please confirm the entered information:"
        echo "AWS CLI profile name: $aws_profile"
        echo "S3 bucket name: $s3_bucket"
        echo "S3 endpoint URL: $s3_endpoint"
        echo "Files to transfer:"
        IFS=' ' read -ra file_list <<< "$file_path"
        for file in "${file_list[@]}"; do
          echo "  - $file"
        done
        echo

        read -ep "Is the information correct? [y/n]: " confirmation

        if [[ "$confirmation" =~ ^[yY] ]]; then
          break
        else
          echo
          echo "Please re-enter the information."
          echo
        fi
      done

      for single_file in $file_path; do
        echo "aws s3 cp --profile $aws_profile --endpoint-url $s3_endpoint $single_file s3://$s3_bucket/"
        aws s3 cp --profile "$aws_profile" --endpoint-url "$s3_endpoint" "$single_file" "s3://$s3_bucket/"
      done
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
      while true; do
        read -ep "AWS CLI profile name: " aws_profile
        read -ep "S3 bucket name: " s3_bucket
        read -ep "S3 endpoint URL: " s3_endpoint
        read -ep "Remote S3 file path: " remote_s3_file
        read -ep "Local directory to download the file: " local_directory

        echo "AWS CLI profile: $aws_profile"
        echo "S3 bucket name: $s3_bucket"
        echo "S3 endpoint URL: $s3_endpoint"
        echo "Remote S3 file path: $remote_s3_file"
        echo "Local directory: $local_directory"

        read -ep "Are the entered values correct? [y/n]: " confirm_values
        if [[ "$confirm_values" =~ ^[yY] ]]; then
          break
        fi
      done

      echo "aws s3 cp --profile $aws_profile --endpoint-url $s3_endpoint s3://$s3_bucket/$remote_s3_file $local_directory/"
      aws s3 cp --profile "$aws_profile" --endpoint-url "$s3_endpoint" "s3://$s3_bucket/$remote_s3_file" "$local_directory/"
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
      while true; do
        read -ep "Source AWS CLI profile name: " src_aws_profile
        read -ep "Source S3 bucket name: " src_s3_bucket
        read -ep "Source S3 endpoint URL: " src_s3_endpoint
        read -ep "Source S3 object path: " src_s3_object_path
        read -ep "Destination AWS CLI profile name: " dest_aws_profile
        read -ep "Destination S3 bucket name: " dest_s3_bucket
        read -ep "Destination S3 endpoint URL: " dest_s3_endpoint
        read -ep "Destination S3 object path: " dest_s3_object_path

        echo "Source AWS CLI profile: $src_aws_profile"
        echo "Source S3 bucket name: $src_s3_bucket"
        echo "Source S3 endpoint URL: $src_s3_endpoint"
        echo "Source S3 object path: $src_s3_object_path"
        echo "Destination AWS CLI profile: $dest_aws_profile"
        echo "Destination S3 bucket name: $dest_s3_bucket"
        echo "Destination S3 endpoint URL: $dest_s3_endpoint"
        echo "Destination S3 object path: $dest_s3_object_path"

        read -ep "Are the entered values correct? [y/n]: " confirm_values
        if [[ "$confirm_values" =~ ^[yY] ]]; then
          break
        fi
      done

      echo "aws s3 cp --profile $src_aws_profile --endpoint-url $src_s3_endpoint s3://$src_s3_bucket/$src_s3_object_path s3://$dest_s3_bucket/$dest_s3_object_path"
      aws s3 cp --profile "$src_aws_profile" --endpoint-url "$src_s3_endpoint" "s3://$src_s3_bucket/$src_s3_object_path" "s3://$dest_s3_bucket/$dest_s3_object_path"
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
      read -ep "Enter the AWS CLI profile name: " aws_profile
      read -ep "S3 endpoint URL: " s3_endpoint
      echo "aws s3 ls --profile $aws_profile --endpoint-url $s3_endpoint"
      aws s3 ls --profile "$aws_profile" --endpoint-url $s3_endpoint
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
