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
      if [[ "$SSHPASS" = [yY] ]]; then
        sshpass -p "$sshpassword" ssh-copy-id -o StrictHostKeyChecking=no -i $HOME/.ssh/${KEYNAME}.key.pub "$remoteuser@$remotehost" -p "$remoteport"
      else
        ssh-copy-id -i $HOME/.ssh/${KEYNAME}.key.pub "$remoteuser@$remotehost" -p "$remoteport"
      fi
      SSHCOPYERR=$?
      if [[ "$SSHCOPYERR" -ne '0' ]]; then
        rm -rf "$HOME/.ssh/${KEYNAME}.key"
        rm -rf "$HOME/.ssh/${KEYNAME}.key.pub"
      fi
      popd >/dev/null 2>&1
    fi
    if [[ "$keyrotate" = 'rotate' ]]; then
      rm -rf "$HOME/.ssh/${KEYNAME}-old.key"
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
    echo "-------------------------------------------------------------------------"
    echo "  $0 {rotatekeys}"
    echo "  $0 {rotatekeys} keytype remoteip remoteport remoteuser keycomment keyname"
    echo
    echo "or"
    echo
    echo "  $0 {rotatekeys} keytype remoteip remoteport remoteuser keycomment \"\" unique_keyname_filename"
    echo
    echo "-------------------------------------------------------------------------"
    echo "  keytype supported: rsa, ecdsa, ed25519"
        ;;
esac