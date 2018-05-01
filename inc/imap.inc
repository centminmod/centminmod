phpimap_install() {
  if [[ "$CENTOS_SEVEN" = '7' && "$PHPIMAP" = [yY] ]]; then
    if [[ $(rpm -q uw-imap-devel >/dev/null 2>&1; echo $?) != '0' ]]; then 
      if [ -f /etc/yum.repos.d/rpmforge.repo ]; then
        time yum${CACHESKIP} -y install uw-imap-devel --disablerepo=rpmforge
        sar_call
      else
        time yum${CACHESKIP} -y install uw-imap-devel
        sar_call
      fi
    fi
  else
    if [[ $(rpm -q libc-client-devel >/dev/null 2>&1; echo $?) != '0' && "$PHPIMAP" = [yY] ]]; then 
      if [ -f /etc/yum.repos.d/rpmforge.repo ]; then
        time yum${CACHESKIP} -y install libc-client-devel --disablerepo=rpmforge,epel
        sar_call
      else
        time yum${CACHESKIP} -y install libc-client-devel --disablerepo=epel
        sar_call
      fi
    fi
  fi
}