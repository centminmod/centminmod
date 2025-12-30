#!/bin/bash
################################################################
# ssh private key pair generator for centminmod.com lemp stacks
################################################################
# ssh-keygen -t rsa or ecdsa
KEYTYPE='rsa'
KEYNAME='my1'

RSA_KEYLENTGH='4096'
ECDSA_KEYLENTGH='256'

KEYGEN_DIR='/etc/keygen'
KEYGEN_LOGDIR="${KEYGEN_DIR}/logs"
DT=$(date +"%d%m%y-%H%M%S")
################################################################
if [ ! -d "$KEYGEN_DIR" ]; then
  mkdir -p "$KEYGEN_DIR"
fi

if [ ! -d "$KEYGEN_LOGDIR" ]; then
  mkdir -p "$KEYGEN_LOGDIR"
fi

# Redirect output of this script log file
exec &> >(tee -a "${KEYGEN_LOGDIR}/keygen-${DT}.log")

if [ ! -d "$HOME/.ssh" ]; then
  mkdir -p "$HOME/.ssh"
  chmod 700 "$HOME/.ssh"
fi

if [ ! -f /usr/bin/sshpass ]; then
  yum -q -y install sshpass >/dev/null 2>&1
  SSHPASS='y'
elif [ -f /usr/bin/sshpass ]; then
  SSHPASS='y'
fi

################################################################
# Enable root SSH login on remote OVH/Cloud VPS
# Used when cloud provider disables root login by default
################################################################
enable_root_login() {
    local remotehost=${_input_remoteh}
    local remoteport=${_input_remotep:-22}
    local sudo_user=${_input_sudo_user}
    local sudo_pass=${_input_sudo_pass}
    local root_pass=${_input_root_pass}

    echo
    echo "-------------------------------------------------------------------"
    echo "Enabling root SSH login on remote host: $remotehost"
    echo "-------------------------------------------------------------------"

    if [[ -z "$remotehost" || -z "$sudo_user" || -z "$sudo_pass" ]]; then
        echo "Error: Missing required parameters"
        echo "Usage: $0 enable-root <remoteip> <port> <sudo_user> <sudo_pass> [root_pass]"
        return 1
    fi

    # Test sudo user connection first
    echo "Testing connection as $sudo_user@$remotehost..."
    if ! sshpass -p "$sudo_pass" ssh -o StrictHostKeyChecking=no -o ConnectTimeout=10 \
        "$sudo_user@$remotehost" -p "$remoteport" "echo 'Connection successful'" 2>/dev/null; then
        echo "Error: Cannot connect as $sudo_user@$remotehost"
        return 1
    fi

    echo "Enabling PermitRootLogin in sshd_config..."
    sshpass -p "$sudo_pass" ssh -o StrictHostKeyChecking=no "$sudo_user@$remotehost" -p "$remoteport" \
        "echo '$sudo_pass' | sudo -S bash -c '
            # Backup sshd_config
            cp /etc/ssh/sshd_config /etc/ssh/sshd_config.bak.\$(date +%Y%m%d%H%M%S)

            # Enable PermitRootLogin
            if grep -q \"^#*PermitRootLogin\" /etc/ssh/sshd_config; then
                sed -i \"s/^#*PermitRootLogin.*/PermitRootLogin yes/\" /etc/ssh/sshd_config
            else
                echo \"PermitRootLogin yes\" >> /etc/ssh/sshd_config
            fi

            echo \"PermitRootLogin enabled\"
        '"

    # Set root password if provided
    if [[ -n "$root_pass" ]]; then
        echo "Setting root password..."
        sshpass -p "$sudo_pass" ssh -o StrictHostKeyChecking=no "$sudo_user@$remotehost" -p "$remoteport" \
            "echo '$sudo_pass' | sudo -S bash -c 'echo \"root:$root_pass\" | chpasswd && echo \"Root password set\"'"
    fi

    # Restart sshd
    echo "Restarting sshd service..."
    sshpass -p "$sudo_pass" ssh -o StrictHostKeyChecking=no "$sudo_user@$remotehost" -p "$remoteport" \
        "echo '$sudo_pass' | sudo -S systemctl restart sshd"

    echo
    echo "-------------------------------------------------------------------"
    echo "Root login enabled on $remotehost"
    echo "You can now use ssh-copy-id to root@$remotehost"
    echo "-------------------------------------------------------------------"
}

################################################################
# Copy SSH key to remote host via sudo user
# Used when direct root login is disabled on cloud VPS
################################################################
sudo_copy_key() {
    local pubkey_file="$1"
    local remotehost="$2"
    local remoteport="$3"
    local target_user="$4"
    local sudo_user="$5"
    local sudo_pass="$6"
    local enable_root_prompt="$7"

    local pubkey=$(cat "$pubkey_file")
    local enable_root=""
    local root_pass=""

    echo
    echo "-------------------------------------------------------------------"
    echo "Copying SSH key via sudo user: $sudo_user"
    echo "Target user: $target_user"
    echo "-------------------------------------------------------------------"

    # Ask about enabling root login if prompted
    if [[ "$enable_root_prompt" = 'y' && "$target_user" = 'root' ]]; then
        read -rep "Also enable PermitRootLogin in sshd_config? [y/n]: " enable_root
        if [[ "$enable_root" = [yY] ]]; then
            read -sep "Enter new root password (leave empty to skip): " root_pass
            echo
        fi
    fi

    # Copy key to target user's authorized_keys
    echo "Copying public key to $target_user@$remotehost..."
    sshpass -p "$sudo_pass" ssh -o StrictHostKeyChecking=no "$sudo_user@$remotehost" -p "$remoteport" \
        "echo '$sudo_pass' | sudo -S bash -c '
            if [[ \"$target_user\" = \"root\" ]]; then
                target_dir=\"/root/.ssh\"
            else
                target_dir=\"/home/$target_user/.ssh\"
            fi

            mkdir -p \"\$target_dir\"
            chmod 700 \"\$target_dir\"

            # Check if key already exists
            if ! grep -qF \"$pubkey\" \"\$target_dir/authorized_keys\" 2>/dev/null; then
                echo \"$pubkey\" >> \"\$target_dir/authorized_keys\"
                chmod 600 \"\$target_dir/authorized_keys\"
                if [[ \"$target_user\" = \"root\" ]]; then
                    chown -R root:root \"\$target_dir\"
                else
                    chown -R $target_user:\$(id -gn $target_user 2>/dev/null || echo $target_user) \"\$target_dir\"
                fi
                echo \"SSH key added successfully\"
            else
                echo \"SSH key already exists in authorized_keys\"
            fi
        '"

    SUDO_COPY_ERR=$?

    # Enable root login if requested
    if [[ "$enable_root" = [yY] && "$target_user" = 'root' ]]; then
        echo "Enabling PermitRootLogin..."
        sshpass -p "$sudo_pass" ssh -o StrictHostKeyChecking=no "$sudo_user@$remotehost" -p "$remoteport" \
            "echo '$sudo_pass' | sudo -S bash -c '
                cp /etc/ssh/sshd_config /etc/ssh/sshd_config.bak.\$(date +%Y%m%d%H%M%S)
                if grep -q \"^#*PermitRootLogin\" /etc/ssh/sshd_config; then
                    sed -i \"s/^#*PermitRootLogin.*/PermitRootLogin yes/\" /etc/ssh/sshd_config
                else
                    echo \"PermitRootLogin yes\" >> /etc/ssh/sshd_config
                fi
            '"

        # Set root password if provided
        if [[ -n "$root_pass" ]]; then
            sshpass -p "$sudo_pass" ssh -o StrictHostKeyChecking=no "$sudo_user@$remotehost" -p "$remoteport" \
                "echo '$sudo_pass' | sudo -S bash -c 'echo \"root:$root_pass\" | chpasswd'"
        fi

        # Restart sshd
        sshpass -p "$sudo_pass" ssh -o StrictHostKeyChecking=no "$sudo_user@$remotehost" -p "$remoteport" \
            "echo '$sudo_pass' | sudo -S systemctl restart sshd"

        echo "PermitRootLogin enabled and sshd restarted"
    fi

    return $SUDO_COPY_ERR
}

keygen() {
    keyrotate=$1
    _keytype=$_input_keytype
    _remoteh=$_input_remoteh
    _remotep=$_input_remotep
    _remoteu=$_input_remoteu
    _comment=$_input_comment
    _sshpass=$_input_sshpass
    _keyname=$_input_keyname
    _unique_keyname=$_input_unique_keyname

    # Modify the KEYNAME generation with the unique key name if provided
    if [[ -n "$_unique_keyname" ]]; then
      KEYNAME="${_unique_keyname}"
    fi

    if [[ $_keytype = 'rsa' ]]; then
      KEYTYPE=$_keytype
      KEYOPT="-t rsa -b $RSA_KEYLENTGH"
    elif [[ $_keytype = 'ecdsa' ]]; then
      KEYTYPE=$_keytype
      KEYOPT="-t ecdsa -b $ECDSA_KEYLENTGH"
    elif [[ $_keytype = 'ed25519' ]]; then
      # openssh 6.7+ supports curve25519-sha256 cipher
      KEYTYPE=$_keytype
      KEYOPT='-t ed25519'
    elif [ -z "$_keytype" ]; then
      KEYTYPE="$KEYTYPE"
        if [[ "$KEYTYPE" = 'rsa' ]]; then
            KEYOPT="-t rsa -b $RSA_KEYLENTGH"
        elif [[ "$KEYTYPE" = 'ecdsa' ]]; then
            KEYOPT="-t ecdsa -b $ECDSA_KEYLENTGH"
        elif [[ "$KEYTYPE" = 'ed25519' ]]; then
            # openssh 6.7+ supports curve25519-sha256 cipher
            KEYOPT='-t ed25519'    
        fi
    fi
    if [[ "$keyrotate" = 'rotate' ]]; then
      echo
      echo "-------------------------------------------------------------------"
      echo "Rotating Private Key Pair..."
      echo "-------------------------------------------------------------------"
      KEYNAME="$_keyname"
      # move existing key pair to still be able to use it
      echo "mv $HOME/.ssh/${KEYNAME}.key $HOME/.ssh/${KEYNAME}-old.key"
      mv "$HOME/.ssh/${KEYNAME}.key" "$HOME/.ssh/${KEYNAME}-old.key"
      echo "mv $HOME/.ssh/${KEYNAME}.key.pub $HOME/.ssh/${KEYNAME}-old.key.pub"
      mv "$HOME/.ssh/${KEYNAME}.key.pub" "$HOME/.ssh/${KEYNAME}-old.key.pub"
    else
      echo
      echo "-------------------------------------------------------------------"
      echo "Generating Private Key Pair..."
      echo "-------------------------------------------------------------------"
      while [ -f "$HOME/.ssh/${KEYNAME}.key" ]; do
          NUM=$(echo "$KEYNAME" | tr -cd '[[:digit:]]') # Extract digits from the key name
          INCREMENT=$(echo $(($NUM+1)))
          if [[ -n "$_unique_keyname" ]]; then
              # Remove digits from the end of the _unique_keyname and add the incremented number
              KEYNAME="$(echo "${_unique_keyname}" | sed 's/[[:digit:]]*$//')${INCREMENT}"
          else
              KEYNAME="my${INCREMENT}"
          fi
      done
    fi
    if [ -z "$_comment" ]; then
      read -rep "enter comment description for key: " keycomment
    else
      keycomment=$_comment
    fi
    echo "ssh-keygen $KEYOPT -N \"\" -f $HOME/.ssh/${KEYNAME}.key -C \"$keycomment\""
    ssh-keygen $KEYOPT -N "" -f $HOME/.ssh/${KEYNAME}.key -C "$keycomment"

    if [[ "$keyrotate" = 'rotate' ]]; then
      OLDPUBKEY=$(cat "$HOME/.ssh/${KEYNAME}-old.key.pub")
      NEWPUBKEY=$(cat "$HOME/.ssh/${KEYNAME}.key.pub")
    fi

    echo
    echo "-------------------------------------------------------------------"
    echo "${KEYNAME}.key.pub public key"
    echo "-------------------------------------------------------------------"
    echo "ssh-keygen -lf $HOME/.ssh/${KEYNAME}.key.pub"
    echo "[size --------------- fingerprint ---------------     - comment - type]"
    echo " $(ssh-keygen -lf $HOME/.ssh/${KEYNAME}.key.pub)"
    
    echo
    echo "cat $HOME/.ssh/${KEYNAME}.key.pub"
    cat "$HOME/.ssh/${KEYNAME}.key.pub"
    
    echo
    echo "-------------------------------------------------------------------"
    echo "$HOME/.ssh contents" 
    echo "-------------------------------------------------------------------"
    ls -lahrt "$HOME/.ssh"

    echo
    echo "-------------------------------------------------------------------"
    echo "Add SSH key to SSH Agent" 
    echo "-------------------------------------------------------------------"
    # add SSH key to SSH Agent
    echo "eval \"$(ssh-agent -s)\""
    eval "$(ssh-agent -s)"
    echo "ssh-add \"$HOME/.ssh/${KEYNAME}.key\""
    ssh-add "$HOME/.ssh/${KEYNAME}.key"

    echo
    echo "-------------------------------------------------------------------"
    echo "Transfering ${KEYNAME}.key.pub to remote host"
    echo "-------------------------------------------------------------------"
    if [ -z "$_remoteh" ]; then
      read -rep "enter remote ip address or hostname: " remotehost
    else
      remotehost=$_remoteh
    fi
    if [ -z "$_remotep" ]; then
      read -rep "enter remote ip/host port number i.e. 22: " remoteport
    else
      remoteport=$_remotep
    fi
    if [ -z "$_remoteu" ]; then
      read -rep "enter remote ip/host username i.e. root: " remoteuser
    else
      remoteuser=$_remoteu
    fi
    if [[ "$SSHPASS" = [yY] ]]; then
      if [[ -z $_sshpass && "$keyrotate" != 'rotate' ]]; then
        read -rep "enter remote ip/host username SSH password: " sshpassword
      else
        sshpassword=$_sshpass
      fi
    fi
    if [[ "$(ping -c1 "$remotehost" -W 2 >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
        VALIDREMOTE=y
      if [[ "$keyrotate" != 'rotate' ]]; then
        echo
        echo "-------------------------------------------------------------------"
        echo "you MAYBE prompted for remote ip/host password"
        echo "enter below command to copy key to remote ip/host"
        echo "-------------------------------------------------------------------"
        echo
      else
        echo
      fi 
    else
      echo
      echo "-------------------------------------------------------------------"
      echo "enter below command to copy key to remote ip/host"
      echo "-------------------------------------------------------------------"
      echo 
    fi
    if [[ "$SSHPASS" = [yY] ]]; then
      if [[ "$keyrotate" = 'rotate' ]]; then
        # rotate key routine replace old remote public key first using renamed
        # $HOME/.ssh/${KEYNAME}-old.key identity
        echo "rotate and replace old public key from remote: $remoteuser@$remotehost"
        echo
        echo "ssh $remoteuser@$remotehost -p $remoteport -i $HOME/.ssh/${KEYNAME}-old.key \"sed -i 's|$OLDPUBKEY|$NEWPUBKEY|' /root/.ssh/authorized_keys\"" | tee "${KEYGEN_LOGDIR}/cmd-rotatekeys-${KEYNAME}-old.key.log"
        echo
        ssh "$remoteuser@$remotehost" -p "$remoteport" -i $HOME/.ssh/${KEYNAME}-old.key "sed -i 's|$OLDPUBKEY|$NEWPUBKEY|' /root/.ssh/authorized_keys"
      else
        echo "copy $HOME/.ssh/${KEYNAME}.key.pub to remote: $remoteuser@$remotehost"
        echo "sshpass -p $sshpassword ssh-copy-id -o StrictHostKeyChecking=no -i $HOME/.ssh/${KEYNAME}.key.pub $remoteuser@$remotehost -p $remoteport" | tee "${KEYGEN_LOGDIR}/cmd-generated-${KEYNAME}.key.log"
      fi
    else
      if [[ "$keyrotate" = 'rotate' ]]; then
        # rotate key routine replace old remote public key first using renamed
        # $HOME/.ssh/${KEYNAME}-old.key identity
        echo "rotate and replace old public key from remote: "$remoteuser@$remotehost""
        echo
        echo "ssh $remoteuser@$remotehost -p $remoteport -i $HOME/.ssh/${KEYNAME}-old.key \"sed -i 's|$OLDPUBKEY|$NEWPUBKEY|' /root/.ssh/authorized_keys\"" | tee "${KEYGEN_LOGDIR}/cmd-rotatekeys-${KEYNAME}-old.key.log"
        echo
        ssh "$remoteuser@$remotehost" -p "$remoteport" -i $HOME/.ssh/${KEYNAME}-old.key "sed -i 's|$OLDPUBKEY|$NEWPUBKEY|' /root/.ssh/authorized_keys"
      else
        echo "copy $HOME/.ssh/${KEYNAME}.key.pub to remote: $remoteuser@$remotehost" | tee "${KEYGEN_LOGDIR}/cmd-generated-${KEYNAME}.key.log"
        echo "ssh-copy-id -i $HOME/.ssh/${KEYNAME}.key.pub $remoteuser@$remotehost -p $remoteport"
      fi
    fi
    if [[ "$VALIDREMOTE" = 'y' && "$keyrotate" != 'rotate' ]]; then
      pushd "$HOME/.ssh" >/dev/null 2>&1
      # Check if sudo user mode is enabled (for OVH/cloud VPS with root login disabled)
      if [[ -n "$_sudo_user" ]]; then
        sudo_copy_key "$HOME/.ssh/${KEYNAME}.key.pub" "$remotehost" "$remoteport" "$remoteuser" "$_sudo_user" "$_sudo_pass" "y"
        SSHCOPYERR=$?
      elif [[ "$SSHPASS" = [yY] ]]; then
        sshpass -p "$sshpassword" ssh-copy-id -o StrictHostKeyChecking=no -i $HOME/.ssh/${KEYNAME}.key.pub "$remoteuser@$remotehost" -p "$remoteport"
        SSHCOPYERR=$?
      else
        ssh-copy-id -i $HOME/.ssh/${KEYNAME}.key.pub "$remoteuser@$remotehost" -p "$remoteport"
        SSHCOPYERR=$?
      fi
      if [[ "$SSHCOPYERR" -ne '0' ]]; then
        echo
        echo "ssh-copy-id transfer failed: removing generated SSH key files"
        echo
        echo "remove $HOME/.ssh/${KEYNAME}.key"
        cat "$HOME/.ssh/${KEYNAME}.key"
        rm -rf "$HOME/.ssh/${KEYNAME}.key"
        echo "remove $HOME/.ssh/${KEYNAME}.key.pub"
        cat "$HOME/.ssh/${KEYNAME}.key.pub"
        rm -rf "$HOME/.ssh/${KEYNAME}.key.pub"
      fi
      popd >/dev/null 2>&1
    fi
    if [[ "$keyrotate" = 'rotate' ]]; then
      echo
      echo "SSH key rotation ssh-copy-id transfer failed: removing generated SSH key files"
      echo
      echo "remove $HOME/.ssh/${KEYNAME}-old.key"
      cat "$HOME/.ssh/${KEYNAME}-old.key"
      rm -rf "$HOME/.ssh/${KEYNAME}-old.key"
      echo "remove $HOME/.ssh/${KEYNAME}-old.key.pub"
      cat "$HOME/.ssh/${KEYNAME}-old.key.pub"
      rm -rf "$HOME/.ssh/${KEYNAME}-old.key.pub"
    fi

    if [[ "$VALIDREMOTE" = 'y' && "$SSHCOPYERR" -eq '0' ]]; then
      echo
      echo "-------------------------------------------------------------------"
      echo "Testing connection please wait..."
      echo "-------------------------------------------------------------------"
      echo
      echo "ssh $remoteuser@$remotehost -p $remoteport -i $HOME/.ssh/${KEYNAME}.key 'uname -nr'"
      echo
      ssh "$remoteuser@$remotehost" -p "$remoteport" -i $HOME/.ssh/${KEYNAME}.key 'uname -nr' | tee "${KEYGEN_LOGDIR}/tmpfile.log"

      ssh_err=$?
      if [[ "$ssh_err" -eq '0' ]]; then
        # log on success
        if [[ "$keyrotate" = 'rotate' ]]; then
          menuopt=rotate
        else
          menuopt=generate
        fi
        sshremote_idname=$(cat "${KEYGEN_LOGDIR}/tmpfile.log")
        rm -rf "${KEYGEN_LOGDIR}/tmpfile.log"
        echo "ip: ${remotehost} user: ${remoteuser} keyname: ${KEYNAME} host: ${sshremote_idname}" > "${KEYGEN_DIR}/${menuopt}-${remotehost}-${remoteport}-${KEYNAME}-${DT}.log"
      fi

      echo
      echo "-------------------------------------------------------------------"
      echo "Setup source server file ${HOME}/.ssh/config"
      echo "-------------------------------------------------------------------"
      echo
      echo "Add to ${HOME}/.ssh/config:"
      echo "Host ${KEYNAME}
        Hostname $remotehost
        Port $remoteport
        IdentityFile $HOME/.ssh/${KEYNAME}.key
        IdentitiesOnly=yes
        User $(id -u -n)
        #LogLevel DEBUG3" | tee "${KEYGEN_LOGDIR}/ssh-config-alias-${KEYNAME}-${remotehost}.key.log"
      echo
      echo "saved copy at ${KEYGEN_LOGDIR}/ssh-config-alias-${KEYNAME}-${remotehost}.key.log"
      echo
      echo "cat ${KEYGEN_LOGDIR}/ssh-config-alias-${KEYNAME}-${remotehost}.key.log >> ${HOME}/.ssh/config"
      echo
      echo "-------------------------------------------------------------------"
      echo "Once ${HOME}/.ssh/config entry added, can connect via Host label:"
      echo " ${KEYNAME}"
      echo "-------------------------------------------------------------------"
      echo
      echo "ssh ${KEYNAME}"
      echo
      echo "-------------------------------------------------------------------"
      echo "keygen.sh run logged to: ${KEYGEN_LOGDIR}/keygen-${DT}.log"
      echo "config logged to: ${KEYGEN_DIR}/${menuopt}-${remotehost}-${remoteport}-${KEYNAME}-${DT}.log"
      echo
      echo "-------------------------------------------------------------------"
      echo "getpk=\$(cat \"$HOME/.ssh/${KEYNAME}.key.pub\")" > "${KEYGEN_LOGDIR}/populate-keygen-${DT}.log"
      echo "if [[ ! \$(grep -w \"\$getpk\" "$HOME/.ssh/authorized_keys") ]]; then cat \"$HOME/.ssh/${KEYNAME}.key.pub\" >> $HOME/.ssh/authorized_keys; fi" >> "${KEYGEN_LOGDIR}/populate-keygen-${DT}.log"
      echo "./sshtransfer.sh $HOME/.ssh/${KEYNAME}.key $remotehost $remoteport ${KEYNAME}.key $HOME/.ssh/" >> "${KEYGEN_LOGDIR}/populate-keygen-${DT}.log"
      echo "populating SSH key file at: ${KEYGEN_LOGDIR}/populate-keygen-${DT}.log"
      echo
      echo "To configure remote with same generated SSH Key:"
      echo "bash ${KEYGEN_LOGDIR}/populate-keygen-${DT}.log"
      echo
      echo "-------------------------------------------------------------------"
      echo "list $KEYGEN_DIR"
      echo
      ls -lAhrt "$KEYGEN_DIR"
      exit
    fi
}

case "$1" in
    gen )
    _input_keytype=$2
    _input_remoteh=$3
    _input_remotep=$4
    _input_remoteu=$5
    _input_comment=$6
    _input_sshpass=$7
    _input_unique_keyname=$8
    _sudo_user=$9           # Sudo user for OVH/cloud VPS with root login disabled
    _sudo_pass=${10}        # Sudo user password
    keygen
    exit
        ;;
    rotatekeys )
    _input_keytype=$2
    _input_remoteh=$3
    _input_remotep=$4
    _input_remoteu=$5
    _input_comment=$6
    _input_keyname=$7
    _input_unique_keyname=$8
    keygen rotate
    exit
        ;;
    enable-root )
    # Enable root SSH login on remote OVH/Cloud VPS
    _input_remoteh=$2
    _input_remotep=$3
    _input_sudo_user=$4
    _input_sudo_pass=$5
    _input_root_pass=$6
    enable_root_login
    exit
        ;;
    * )
    echo "-------------------------------------------------------------------------"
    echo "  $0 {gen}"
    echo "  $0 {gen} keytype remoteip remoteport remoteuser keycomment"
    echo
    echo "  or"
    echo
    echo "  $0 {gen} keytype remoteip remoteport remoteuser keycomment remotessh_password"
    echo
    echo "  or"
    echo
    echo "  $0 {gen} keytype remoteip remoteport remoteuser keycomment remotessh_password unique_keyname_filename"
    echo
    echo "  or (for OVH/cloud VPS with root login disabled)"
    echo
    echo "  $0 {gen} keytype remoteip remoteport remoteuser keycomment remotessh_password unique_keyname_filename sudo_user sudo_pass"
    echo
    echo "-------------------------------------------------------------------------"
    echo "  $0 {rotatekeys}"
    echo "  $0 {rotatekeys} keytype remoteip remoteport remoteuser keycomment keyname"
    echo
    echo "or"
    echo
    echo "  $0 {rotatekeys} keytype remoteip remoteport remoteuser keycomment \"\" unique_keyname_filename"
    echo
    echo "-------------------------------------------------------------------------"
    echo "  $0 {enable-root}"
    echo "  $0 {enable-root} remoteip remoteport sudo_user sudo_pass [root_pass]"
    echo
    echo "  Enable root SSH login on remote OVH/Cloud VPS"
    echo "  sudo_user: almalinux, rocky, cloud-user, opc (Oracle Linux)"
    echo
    echo "-------------------------------------------------------------------------"
    echo "  keytype supported: rsa, ecdsa, ed25519"
        ;;
esac