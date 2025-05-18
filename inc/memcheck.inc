# If set to yes, will abort centmin mod installation if the memory requirements are not met
# you can override this setting by setting ABORTINSTALL='n' in which case centmin mod
# install may either install successfully but very very slowly or crap out
# and fail to successfully install.
ABORTINSTALL='y'

#############################################################
TOTALMEM=$(awk '/MemTotal/ {print $2}' /proc/meminfo)
TOTALMEM_T=$(awk '/MemTotal/ {print $2}' /proc/meminfo)
TOTALMEM_SWAP=$(awk '/SwapFree/ {print $2}' /proc/meminfo)
TOTALMEM_PHP=$(($TOTALMEM_T+$TOTALMEM_SWAP))

if [[ "$CENTOS_EIGHT" -eq '8' || "$CENTOS_NINE" -eq '9' || "$CENTOS_TEN" -eq '10' ]]; then
  if [[ "$ISMINMEM_OVERRIDE" = [yY] ]]; then
    ISMINMEM='1500000'  # 1.43GB in bytes
  else
    ISMINMEM='1730000'  # 1.7GB in bytes
  fi
  if [[ "$ISMINSWAP_OVERRIDE" = [yY] ]]; then
    ISMINSWAP='2097152'  # 2.0GB in bytes
  else
    ISMINSWAP='3774873'  # 3.6GB in bytes
  fi
elif [[ "$CENTOS_SEVEN" -eq '7' ]]; then
  ISMINMEM='922624'  # 900MB in bytes
  ISMINSWAP='2097152'  # 2GB in bytes
else
  ISMINMEM='262144'  # 256MB in bytes
  ISMINSWAP='524288'  # 512MB in bytes
fi

#############################################################
# Formulas
TOTALMEMMB=`echo "scale=0;$TOTALMEM/1024" | bc`
ISMINMEMMB=`echo "scale=0;$ISMINMEM/1024" | bc`
ISMINSWAPMB=`echo "scale=0;$ISMINSWAP/1024" | bc`
CHECKMINMEM=`expr $TOTALMEM_T \< $ISMINMEM`

#############################################################
lowmemcheck() {
  MEMFIRSTINSTALL=$1
  if [[ "$MEMFIRSTINSTALL" = 'initialinstall' ]]; then
    INITIALINSTALL='y'
  else
    INITIALINSTALL='n'
  fi

  # function checks if persistent config file has low mem variable enabled
  # LOWMEM_INSTALL='y'
  # checkfor_lowmem

  if [[ "$INITIALINSTALL" = [yY] ]]; then
    cpcheck initialinstall
  else
    cpcheck
  fi

  histformat
  expand_cmdprompt

  # Check memory and swap threshold
  if [ "$CHECKMINMEM" == "1" ]; then
    if [ "$TOTALMEM_SWAP" -lt "$ISMINSWAP" ]; then
      CPUS='1'
      MAKETHREADS=" -j$CPUS"
      echo ""
      if [[ "$CENTOS_EIGHT" -eq '8' || "$CENTOS_NINE" -eq '9' || "$CENTOS_TEN" -eq '10' ]]; then
        echo "For EL8, EL9, EL10 operating system the minimum and recommended memory requirements have increased"
        echo "Minium: 2GB memory with 4GB swap disk"
        echo "Recommended: 4GB memory with 4GB swap disk"
      fi
      echo -e "Warning: physically installed memory and swap too low for Centmin Mod\\nInstallation [Installed: $TOTALMEMMB MB < $ISMINMEMMB MB memory and $ISMINSWAPMB MB < $ISMINSWAPMB MB swap (recommended minimum)]\\n"
      if [ "$ABORTINSTALL" == 'y' ]; then
        echo "aborting install..."
        sleep 20
        exit
      fi
    else
      echo ""
      echo -e "Ok: swap is sufficient for Centmin Mod installation despite low memory\\nInstallation [Installed: $TOTALMEMMB MB < $ISMINMEMMB MB memory, but $ISMINSWAPMB MB >= $ISMINSWAPMB MB swap]\\n"
    fi
  else
    echo ""
    echo -e "Ok: physically installed memory is sufficient for Centmin Mod\\nInstallation [Installed: $TOTALMEMMB MB >= $ISMINMEMMB MB memory]\\n"
  fi
}