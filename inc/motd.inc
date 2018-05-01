checkmotd() {
      if [[ -z "$(grep 'PrintMotd no' /etc/ssh/sshd_config)" ]]; then
        echo "" >> /etc/ssh/sshd_config
        echo "PrintMotd no" >> /etc/ssh/sshd_config
        \cp -f /etc/motd /etc/motd-backup1
        echo > /etc/motd
        service sshd reload
      fi
      # remove dmotd from /etc/profile to /etc/profile.d/dmotd.sh
      if [[ "$(grep '\/usr\/local\/bin\/dmotd' /etc/profile)" ]]; then
        sed -i '/\/usr\/local\/bin\/dmotd/d' /etc/profile
      fi
      if [ ! -f /etc/profile.d/dmotd.sh ]; then
        echo '#!/bin/bash' > /etc/profile.d/dmotd.sh
        echo "if [[ \"$(id -u)\" = '0' && -f /usr/local/bin/dmotd ]]; then  /usr/local/bin/dmotd; fi" >> /etc/profile.d/dmotd.sh
        \cp -f /etc/motd /etc/motd-backup2
        echo > /etc/motd
      fi
      # fix line breaks if incorrect
      sed -i 's|sha1PrintMotd no|sha1\nPrintMotd no|' /etc/ssh/sshd_config
      sed -i 's|UseDNS noPrintMotd no|UseDNS no\nPrintMotd no|' /etc/ssh/sshd_config
}

setupmotd() {

  ENABLEMOTD='y'
  CUR_DIR=$SCRIPT_DIR # Get current directory.
  CM_INSTALLDIR=$CUR_DIR

  if [ -f "${CM_INSTALLDIR}/inc/custom_config.inc" ]; then
      source "inc/custom_config.inc"
  fi
  
  if [ -f "${CONFIGSCANBASE}/custom_config.inc" ]; then
      # default is at /etc/centminmod/custom_config.inc
      source "${CONFIGSCANBASE}/custom_config.inc"
  fi
  
  if [ -f "${CM_INSTALLDIR}/inc/z_custom.inc" ]; then
      source "${CM_INSTALLDIR}/inc/z_custom.inc"
  fi

  if [[ "$ENABLEMOTD" = [yY] ]]; then
    if [ -f /usr/local/bin/dmotd ]; then
      rm -rf /usr/local/bin/dmotd
      if [ -f "${SCRIPT_DIR}/config/motd/dmotd.sh" ]; then
        ln -s "${SCRIPT_DIR}/config/motd/dmotd.sh" /usr/local/bin/dmotd >/dev/null 2>&1
        if [ -f /usr/local/bin/dmotd ]; then
          chmod +x /usr/local/bin/dmotd
        fi
        checkmotd
      fi
    else
      if [ -f "${SCRIPT_DIR}/config/motd/dmotd.sh" ]; then
        ln -s "${SCRIPT_DIR}/config/motd/dmotd.sh" /usr/local/bin/dmotd >/dev/null 2>&1
        if [ -f /usr/local/bin/dmotd ]; then
          chmod +x /usr/local/bin/dmotd
        fi
        checkmotd
      fi
    fi
  elif [[ "$ENABLEMOTD" != [yY] ]]; then
    rm -rf /usr/local/bin/dmotd
  fi #ENABLEMOTD

}