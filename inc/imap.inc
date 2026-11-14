# EL8+: retry a failed uw-imap-devel install once with fresh repo metadata, then say why
# PHP's IMAP build will fail. An out-of-sync mirror (2026-11-14: Remi remi-safe served
# libc-client 2007f-33 while the remi mirror had only uw-imap-devel 2007f-30) makes dnf
# refuse the pair, and PHP's --with-imap configure later fails with a misleading
# U8T_CANONICAL error.
phpimap_devel_check() {
  local imap_pkgs='libc-client uw-imap-devel' imap_repoopt='--enablerepo=remi'
  rpm -q uw-imap-devel >/dev/null 2>&1 && return 0
  if [[ "$CENTOS_EIGHT" = '8' ]]; then
    imap_pkgs='uw-imap-devel'
    imap_repoopt=''
  fi
  if [ -f /etc/yum.repos.d/rpmforge.repo ]; then
    imap_repoopt='--disablerepo=rpmforge'
  fi
  cecho "uw-imap-devel is not installed, retrying with refreshed repo metadata" $boldyellow
  time yum${CACHESKIP} -y install ${imap_pkgs} ${imap_repoopt} --refresh --setopt=skip_if_unavailable=True
  sar_call
  rpm -q uw-imap-devel >/dev/null 2>&1 && return 0
  cecho "Error: uw-imap-devel could not be installed, so PHP's IMAP extension can't be built." $boldyellow
  cecho "Likely cause: libc-client and uw-imap-devel are at different releases in the enabled repos (an out-of-sync mirror). See the dnf error above." $boldyellow
  cecho "PHP versions before 8.4 will then fail configure with: utf8_mime2text() has new signature, but U8T_CANONICAL is missing" $boldyellow
  cecho "Retry later, or set PHPIMAP='n' in /etc/centminmod/custom_config.inc to build PHP without IMAP." $boldyellow
  return 1
}

phpimap_install() {
  if [[ "$CENTOS_NINE" = '9' && "$PHPIMAP" = [yY] ]]; then
    if [[ $(rpm -q uw-imap-devel >/dev/null 2>&1; echo $?) != '0' ]]; then 
      if [ -f /etc/yum.repos.d/rpmforge.repo ]; then
        time yum${CACHESKIP} -y install libc-client uw-imap-devel --disablerepo=rpmforge
        sar_call
      else
        time yum${CACHESKIP} -y install libc-client uw-imap-devel --enablerepo=remi
        sar_call
      fi
    fi
  elif [[ "$CENTOS_EIGHT" = '8' && "$PHPIMAP" = [yY] ]]; then
    if [[ $(rpm -q uw-imap-devel >/dev/null 2>&1; echo $?) != '0' ]]; then 
      if [ -f /etc/yum.repos.d/rpmforge.repo ]; then
        time yum${CACHESKIP} -y install uw-imap-devel --disablerepo=rpmforge
        sar_call
      else
        time yum${CACHESKIP} -y install uw-imap-devel
        sar_call
      fi
    fi
  elif [[ "$CENTOS_SEVEN" = '7' && "$PHPIMAP" = [yY] ]]; then
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
  if [[ "$PHPIMAP" = [yY] ]] && [[ "$CENTOS_EIGHT" = '8' || "$CENTOS_NINE" = '9' ]]; then
    phpimap_devel_check
  fi
}