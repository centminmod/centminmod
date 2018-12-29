#######################################################################################
# Centmin Mod code updater sub menu
#######################################################################################
re_sourcefiles() {
source "${CM_INSTALLDIR}/centmin.sh"
source "${CM_INSTALLDIR}/inc/customrpms.inc"
source "${CM_INSTALLDIR}/inc/pureftpd.inc"
source "${CM_INSTALLDIR}/inc/htpasswdsh.inc"
source "${CM_INSTALLDIR}/inc/gcc.inc"
source "${CM_INSTALLDIR}/inc/entropy.inc"
source "${CM_INSTALLDIR}/inc/cpucount.inc"
source "${CM_INSTALLDIR}/inc/motd.inc"
source "${CM_INSTALLDIR}/inc/cpcheck.inc"
source "${CM_INSTALLDIR}/inc/memcheck.inc"
source "${CM_INSTALLDIR}/inc/ccache.inc"
source "${CM_INSTALLDIR}/inc/bookmark.inc"
source "${CM_INSTALLDIR}/inc/centminlogs.inc"
source "${CM_INSTALLDIR}/inc/yumskip.inc"
source "${CM_INSTALLDIR}/inc/questions.inc"
source "${CM_INSTALLDIR}/inc/downloads_centosfive.inc"
source "${CM_INSTALLDIR}/inc/downloads_centossix.inc"
source "${CM_INSTALLDIR}/inc/downloads_centosseven.inc"
source "${CM_INSTALLDIR}/inc/downloadlinks.inc"
source "${CM_INSTALLDIR}/inc/downloads.inc"
source "${CM_INSTALLDIR}/inc/yumpriorities.inc"
source "${CM_INSTALLDIR}/inc/yuminstall.inc"
source "${CM_INSTALLDIR}/inc/centoscheck.inc"
source "${CM_INSTALLDIR}/inc/axelsetup.inc"
source "${CM_INSTALLDIR}/inc/phpfpmdir.inc"
source "${CM_INSTALLDIR}/inc/nginx_backup.inc"
source "${CM_INSTALLDIR}/inc/nsd_submenu.inc"
source "${CM_INSTALLDIR}/inc/nsd_install.inc"
source "${CM_INSTALLDIR}/inc/nsdsetup.inc"
source "${CM_INSTALLDIR}/inc/nsd_reinstall.inc"
source "${CM_INSTALLDIR}/inc/nginx_logformat.inc"
source "${CM_INSTALLDIR}/inc/logrotate_nginx.inc"
source "${CM_INSTALLDIR}/inc/logrotate_phpfpm.inc"
source "${CM_INSTALLDIR}/inc/nginx_mimetype.inc"
source "${CM_INSTALLDIR}/inc/openssl_install.inc"
source "${CM_INSTALLDIR}/inc/brotli.inc"
source "${CM_INSTALLDIR}/inc/nginx_configure.inc"
source "${CM_INSTALLDIR}/inc/geoip.inc"
source "${CM_INSTALLDIR}/inc/luajit.inc"
source "${CM_INSTALLDIR}/inc/nginx_patch.inc"
source "${CM_INSTALLDIR}/inc/nginx_install.inc"
source "${CM_INSTALLDIR}/inc/nginx_upgrade.inc"
source "${CM_INSTALLDIR}/inc/mailparse.inc"
source "${CM_INSTALLDIR}/inc/imagick_install.inc"
source "${CM_INSTALLDIR}/inc/memcached_install.inc"
source "${CM_INSTALLDIR}/inc/redis_submenu.inc"
source "${CM_INSTALLDIR}/inc/redis.inc"
source "${CM_INSTALLDIR}/inc/mongodb.inc"
source "${CM_INSTALLDIR}/inc/mysql_proclimit.inc"
source "${CM_INSTALLDIR}/inc/mysqltmp.inc"
source "${CM_INSTALLDIR}/inc/setmycnf.inc"
source "${CM_INSTALLDIR}/inc/mariadb_install.inc"
source "${CM_INSTALLDIR}/inc/mysql_install.inc"
source "${CM_INSTALLDIR}/inc/mariadb_submenu.inc"
source "${CM_INSTALLDIR}/inc/postgresql.inc"
if [ -f "${CM_INSTALLDIR}/inc/zendopcache_tweaks.inc" ]; then
  source "${CM_INSTALLDIR}/inc/zendopcache_tweaks.inc"
fi
source "${CM_INSTALLDIR}/inc/php_configure.inc"
source "${CM_INSTALLDIR}/inc/phpng_download.inc"
source "${CM_INSTALLDIR}/inc/php_extraopts.inc"
source "${CM_INSTALLDIR}/inc/php_upgrade.inc"
source "${CM_INSTALLDIR}/inc/php_mssql.inc"
source "${CM_INSTALLDIR}/inc/suhosin_setup.inc"
source "${CM_INSTALLDIR}/inc/nginx_pagespeed.inc"
source "${CM_INSTALLDIR}/inc/nginx_modules.inc"
source "${CM_INSTALLDIR}/inc/nginx_modules_openresty.inc"
source "${CM_INSTALLDIR}/inc/sshd.inc"
source "${CM_INSTALLDIR}/inc/openvz_stack.inc"
source "${CM_INSTALLDIR}/inc/siegeinstall.inc"
source "${CM_INSTALLDIR}/inc/python_install.inc"
source "${CM_INSTALLDIR}/inc/nginx_addvhost.inc"
source "${CM_INSTALLDIR}/inc/wpsetup.inc"
source "${CM_INSTALLDIR}/inc/mariadb_upgrade.inc"
source "${CM_INSTALLDIR}/inc/mariadb_upgrade53.inc"
source "${CM_INSTALLDIR}/inc/mariadb_upgrade55.inc"
source "${CM_INSTALLDIR}/inc/mariadb_upgrade10.inc"
source "${CM_INSTALLDIR}/inc/mariadb_upgrade101.inc"
source "${CM_INSTALLDIR}/inc/nginx_errorpage.inc"
source "${CM_INSTALLDIR}/inc/sendmail.inc"
source "${CM_INSTALLDIR}/inc/postfix.inc"
source "${CM_INSTALLDIR}/inc/compress.inc"
source "${CM_INSTALLDIR}/inc/diskalert.inc"
source "${CM_INSTALLDIR}/inc/phpsededit.inc"
source "${CM_INSTALLDIR}/inc/csfinstall.inc"
source "${CM_INSTALLDIR}/inc/csftweaks.inc"
source "${CM_INSTALLDIR}/inc/xcache_installask.inc"
source "${CM_INSTALLDIR}/inc/xcache_install.inc"
source "${CM_INSTALLDIR}/inc/xcache_reinstall.inc"
source "${CM_INSTALLDIR}/inc/igbinary.inc"
source "${CM_INSTALLDIR}/inc/zlib.inc"
source "${CM_INSTALLDIR}/inc/apcprotect.inc"
source "${CM_INSTALLDIR}/inc/apcinstall.inc"
source "${CM_INSTALLDIR}/inc/apcreinstall.inc"
source "${CM_INSTALLDIR}/inc/zendopcache_55ini.inc"
source "${CM_INSTALLDIR}/inc/zendopcache_install.inc"
source "${CM_INSTALLDIR}/inc/zendopcache_upgrade.inc"
source "${CM_INSTALLDIR}/inc/zendopcache_reinstall.inc"
source "${CM_INSTALLDIR}/inc/zendopcache_submenu.inc"
source "${CM_INSTALLDIR}/inc/ffmpeginstall.inc"
source "${CM_INSTALLDIR}/inc/shortcuts_install.inc"
source "${CM_INSTALLDIR}/inc/memcacheadmin.inc"
source "${CM_INSTALLDIR}/inc/mysqlsecure.inc"
if [ -f "${CM_INSTALLDIR}/inc/letsencrypt.inc" ]; then
  source "${CM_INSTALLDIR}/inc/letsencrypt.inc"
fi
# if [ -f "${CM_INSTALLDIR}/addons/letsencrypt.sh" ]; then
#   source "${CM_INSTALLDIR}/addons/letsencrypt.sh"
# fi
source "${CM_INSTALLDIR}/inc/updater_submenu.inc"
source "${CM_INSTALLDIR}/inc/centminfinish.inc"

if [ -f "${CM_INSTALLDIR}/inc/custom_config.inc" ]; then
    source "${CM_INSTALLDIR}/inc/custom_config.inc"
fi

if [ -f "${CONFIGSCANBASE}/custom_config.inc" ]; then
    # default is at /etc/centminmod/custom_config.inc
    dos2unix -q "${CONFIGSCANBASE}/custom_config.inc"
    source "${CONFIGSCANBASE}/custom_config.inc"
fi

if [ -f "${CM_INSTALLDIR}/inc/z_custom.inc" ]; then
    dos2unix -q "${CM_INSTALLDIR}/inc/z_custom.inc"
    source "${CM_INSTALLDIR}/inc/z_custom.inc"
fi
}

gitenv_setup() {
  # check local DNS resolve and network connectivity before setting up git clone
  LOCALDNS_RESOLVER=$(awk '/nameserver/ {print $0}' /etc/resolv.conf | grep -v '#' | awk 'NR==1 {print $2}')
  if [[ "$LOCALDNS_RESOLVER" = '127.0.0.1' || "$LOCALDNS_RESOLVER" = 'localhost' ]]; then
    LOCALDNS_RESOLVER='8.8.8.8'
  fi
  DNSRESOLVER_CHECKA=$(dig +short A github.com @$LOCALDNS_RESOLVER >/dev/null 2>&1; echo $?)
  DNSRESOLVER_CHECKB=$(dig +short A raw.githubusercontent.com @$LOCALDNS_RESOLVER >/dev/null 2>&1; echo $?)
  if [[ "$(echo ${DNSRESOLVER_CHECKA}${DNSRESOLVER_CHECKA})" != '00' ]]; then
    echo "Error: there seems to be an issues with local DNS resolvers /etc/resolv.conf"
    echo "       or internet connectivity on server"
    echo "ping -c4 github.com"
    ping -c4 github.com
    echo "ping -c4 raw.githubusercontent.com"
    ping -c4 raw.githubusercontent.com
    exit 1
  fi
  # get Centmin Mod Git url remotely so to allow for Git repo renaming in future where
  # user can just use centmin.sh menu option 23 submenus to switch to a newly renamed
  # Git repo name if needed
  REMOTE_CMGIT=$(curl -s${ipv_forceopt} https://raw.githubusercontent.com/centminmod/centminmod/${branchname}/giturl.txt)
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
  echo "time git clone -b ${branchname} --depth=1 ${REMOTE_CMGIT} centminmod"
  time git clone -b ${branchname} --depth=1 ${REMOTE_CMGIT} centminmod
  if [ ! -d centminmod ]; then
    echo "time git clone -b ${branchname} --depth=1 ${REMOTE_CMGIT} centminmod"
    time git clone -b ${branchname} --depth=1 ${REMOTE_CMGIT} centminmod
  fi
  cd centminmod
  chmod +x centmin.sh
  
  echo
  cecho "list all available local branches" $boldyellow
  cecho " git branch -a" $boldgreen
  git branch -a
  
  echo
  cecho "list git log last commit" $boldyellow
  cecho " git log -a" $boldgreen
  git log -1 | sed -e 's|Author: George Liu <.*>|Author: George Liu <snipped>|g'
  
  echo
  cecho "to update centmin mod ${branchname} branch repo via git" $boldyellow
  cecho " cd /usr/local/src/centminmod" $boldgreen
  cecho " git stash" $boldgreen
  cecho " git pull" $boldgreen
  cecho " chmod +x centmin.sh" $boldgreen

  echo
  cecho "##################### IMPORTANT #####################" $boldgreen
  cecho " To complete update " $boldyellow
  cecho "##################### IMPORTANT #####################" $boldgreen
  cecho " run:" $boldyellow
  echo
  echo "  cd /usr/local/src/centminmod"
  echo
  cecho " before invoking centmin.sh again" $boldyellow
  cecho "##################### IMPORTANT #####################" $boldgreen
  echo
  SPID=$$
  # echo "$SPID process id for centmin.sh ended"
  kill "$SPID"
}

gitenv_update() {
  CM_DIRSIZE=$(df -P /usr/local/src/centminmod | awk '{print $4}' | tail -1 | awk '{print $1}')
  REMOTE_CMGIT=$(curl -s${ipv_forceopt} https://raw.githubusercontent.com/centminmod/centminmod/${branchname}/giturl.txt)
if [[ -d "${SCRIPT_DIR}/.git" ]]; then
  if [[ "$(curl -sL https://github.com/centminmod/centminmod/raw/${branchname}/gitclean.txt)" = 'no' ]]; then
    echo
    cecho "Updating Current Centmin Mod code branch via git" $boldyellow
    # git fetch
    # check if there's any new remote commits first
        pushd "${SCRIPT_DIR}" >/dev/null 2>&1
        git fetch >/dev/null 2>&1
        popd >/dev/null 2>&1
    if [[ "$(cd /usr/local/src/centminmod; git rev-parse HEAD)" != "$(cd /usr/local/src/centminmod; git rev-parse @{u})" ]]; then
    cecho " cd /usr/local/src/centminmod" $boldgreen
    cecho " git stash" $boldgreen
    cecho " git pull" $boldgreen
    cecho " chmod +x centmin.sh" $boldgreen
    cd /usr/local/src/centminmod 2>/dev/null
    git stash
    git pull
  
    if [[ "$CM_DIRSIZE" -lt '4000000' ]]; then
      # git reflog expire --all --expire=now
      echo
      git count-objects -v
      echo
      cecho "git gc..." $boldyellow
      echo
      if [ -f git-agg ]; then
        git gc --prune=now
      else
        echo
        cecho "may take a few minutes" $boldyellow
        echo
        # set windowMemory and packSizeLimit to workaround
        # failed repack runs
        TOTALMEMSWAP=$(egrep '^MemTotal|^SwapTotal' /proc/meminfo | awk '{ t+=$2 } END { print t }')
        if [[ "$TOTALMEMSWAP" -le '131072' ]]; then
          git config pack.windowMemory "30m"
          git config pack.packSizeLimit "250m"
        elif [[ "$TOTALMEMSWAP" -le '262144' && "$TOTALMEMSWAP" -ge '131073' ]]; then
          git config pack.windowMemory "50m"
          git config pack.packSizeLimit "250m"
        elif [[ "$TOTALMEMSWAP" -le '524288' && "$TOTALMEMSWAP" -ge '262145' ]]; then
          git config pack.windowMemory "125m"
          git config pack.packSizeLimit "250m"
        elif [[ "$TOTALMEMSWAP" -le '1048576' && "$TOTALMEMSWAP" -ge '524289' ]]; then
          git config pack.windowMemory "250m"
          git config pack.packSizeLimit "250m"
        elif [[ "$TOTALMEMSWAP" -le '2097152' && "$TOTALMEMSWAP" -ge '1048577' ]]; then
          git config pack.windowMemory "350m"
          git config pack.packSizeLimit "500m"
        elif [[ "$TOTALMEMSWAP" -le '4194304' && "$TOTALMEMSWAP" -ge '2097153' ]]; then
          git config pack.windowMemory "500m"
          git config pack.packSizeLimit "1000m"
        elif [[ "$TOTALMEMSWAP" -le '8388607' && "$TOTALMEMSWAP" -ge '4194305' ]]; then
          git config pack.windowMemory "1000m"
          git config pack.packSizeLimit "1000m"
        elif [[ "$TOTALMEMSWAP" -ge '8388608' ]]; then
          git config pack.windowMemory "1500m"
          git config pack.packSizeLimit "1000m"
        fi
        git gc --prune=now --aggressive
        touch git-agg
      fi
      echo
      git count-objects -v
    fi
  
    chmod +x centmin.sh
  
    echo
    cecho "##################### IMPORTANT #####################" $boldgreen
    cecho " To complete update " $boldyellow
    cecho "##################### IMPORTANT #####################" $boldgreen
    cecho " run:" $boldyellow
    echo
    echo "  cd /usr/local/src/centminmod"
    echo
    cecho " before invoking centmin.sh again" $boldyellow
    cecho "##################### IMPORTANT #####################" $boldgreen
    echo
    SPID=$$
    # echo "$SPID process id for centmin.sh ended"
    kill "$SPID"
    # re_sourcefiles
    # break
    else
      # no new commits/updates available
      echo
      cecho " Centmin Mod local code up to date at /usr/local/src/centminmod" $boldyellow
      cecho " no available updates at this time..." $boldyellow
      echo
      SPID=$$
      # echo "$SPID process id for centmin.sh ended"
      kill "$SPID"
    fi
    break 2
  else
    echo
    echo "Detected Centmin Mod Github Remote Repo Changes"
    echo "setting up fresh /usr/local/src/centminmod code base to match"
    echo
    cd /usr/local/src
    rm -rf centminmod
    echo "time git clone -b ${branchname} --depth=1 ${REMOTE_CMGIT} centminmod"
    time git clone -b ${branchname} --depth=1 ${REMOTE_CMGIT} centminmod
    echo
    echo "Completed. Fresh /usr/local/src/centminmod code base in place"
    echo "To run centmin.sh again, you need to change into directory: /usr/local/src/centminmod"
    echo "cd /usr/local/src/centminmod"
    echo
  fi # check gitclean.txt
else
  echo
  echo "Error: you do not have git environment setup for github based updates"
  echo "       run submenu option 1 to setup github environment first"
  break
fi
}

gitenv_change() {
  # check local DNS resolve and network connectivity before setting up git clone
  LOCALDNS_RESOLVER=$(awk '/nameserver/ {print $0}' /etc/resolv.conf | awk 'NR==1 {print $2}')
  if [[ "$LOCALDNS_RESOLVER" = '127.0.0.1' || "$LOCALDNS_RESOLVER" = 'localhost' ]]; then
    LOCALDNS_RESOLVER='8.8.8.8'
  fi
  DNSRESOLVER_CHECKA=$(dig +short A github.com @$LOCALDNS_RESOLVER >/dev/null 2>&1; echo $?)
  DNSRESOLVER_CHECKB=$(dig +short A raw.githubusercontent.com @$LOCALDNS_RESOLVER >/dev/null 2>&1; echo $?)
  if [[ "$(echo ${DNSRESOLVER_CHECKA}${DNSRESOLVER_CHECKA})" != '00' ]]; then
    echo "Error: there seems to be an issues with local DNS resolvers /etc/resolv.conf"
    echo "       or internet connectivity on server"
    echo "ping -c4 github.com"
    ping -c4 github.com
    echo "ping -c4 raw.githubusercontent.com"
    ping -c4 raw.githubusercontent.com
    exit 1
  fi
  CM_DIRSIZE=$(df -P /usr/local/src/centminmod | awk '{print $4}' | tail -1 | awk '{print $1}')
  REMOTE_CMGIT=$(curl -s${ipv_forceopt} https://raw.githubusercontent.com/centminmod/centminmod/${branchname}/giturl.txt)
if [[ -d "${SCRIPT_DIR}/.git" ]]; then
  echo
  echo "fetching latest branch list..."
  git fetch
  echo
  cecho "Update Centmin Mod to newer branch via git" $boldyellow
  cecho "You need to input the name of the branch" $boldyellow
  cecho "List of current remote branches by descending date order" $boldyellow
  echo

  # for k in `git branch -r|sed s/^..//`;do echo -e `git log -1 --pretty=format:"%Cgreen%ci %Cblue%cr%Creset" "$k" 2>/dev/null`\\t"$k";done| sed -e 's|origin\/||g' -e '/HEAD/d' | sort -r
  # grab branch listings using curl
  if [[ "$(echo $CMGIT | grep github)" ]]; then
    GITCURLSTATUS=$(curl -sI https://github.com/centminmod/centminmod/branches/active | grep 'HTTP\/' | awk '/200/ {print $2}')
    if [[ "$GITCURLSTATUS" = '200' ]]; then
      CMBRANCHLIST=$(curl -s${ipv_forceopt} https://github.com/centminmod/centminmod/branches/active | grep -P '\/centminmod\/centminmod\/tree\/.*">' | grep -Po '(?<=href=")[^"]*(?=")' | sed 's|/centminmod/centminmod/tree/||g' | egrep -v '123.06stable|123.07stable|123.08beta03|123.08beta03-rtmp|123.08zerodown|123.09beta01-gcc|123.09beta01-dnf|123.09beta01-nchan')
      echo "$CMBRANCHLIST"
    fi
  else
    GITCURLSTATUS=$(curl -sI https://gitlab.com/centminmod/centminmod/branches?sort=recently_updated | grep 'HTTP\/' | awk '/200/ {print $2}')
    if [[ "$GITCURLSTATUS" = '200' ]]; then
    CMBRANCHLIST=$(curl -s${ipv_forceopt} https://gitlab.com/centminmod/centminmod/branches?sort=recently_updated | grep -P '\/centminmod\/centminmod\/tree\/.*">' | grep -Po '(?<=href=")[^"]*(?=")' | sed 's|/centminmod/centminmod/tree/||g' | egrep -v '123.06stable|123.07stable|123.08beta03|123.08beta03-rtmp|123.08zerodown|123.09beta01-gcc|123.09beta01-dnf|123.09beta01-nchan')
    echo "$CMBRANCHLIST"
    fi
  fi
  
  echo
  read -ep "Enter the branch name you want to switch to i.e. 123.09beta01 : " branchname
  echo

  if [ ! "$(echo $CMBRANCHLIST | grep -o "${branchname}")" ]; then
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
    echo "time git clone -b ${branchname} --depth=1 ${REMOTE_CMGIT} centminmod"
    time git clone -b ${branchname} --depth=1 ${REMOTE_CMGIT} centminmod
    if [ ! -d centminmod ]; then
      echo "time git clone -b ${branchname} --depth=1 ${REMOTE_CMGIT} centminmod"
      time git clone -b ${branchname} --depth=1 ${REMOTE_CMGIT} centminmod
    fi
    cd centminmod 2>/dev/null

    if [[ "$CM_DIRSIZE" -lt '4000000' ]]; then
      echo
      git count-objects -v
      echo
      cecho "git gc..." $boldyellow
      echo
      if [ -f git-agg ]; then
        git gc --prune=now
      else
        echo
        cecho "may take a few minutes" $boldyellow
        echo
        # set windowMemory and packSizeLimit to workaround
        # failed repack runs
        TOTALMEMSWAP=$(egrep '^MemTotal|^SwapTotal' /proc/meminfo | awk '{ t+=$2 } END { print t }')
        if [[ "$TOTALMEMSWAP" -le '131072' ]]; then
          git config pack.windowMemory "30m"
          git config pack.packSizeLimit "250m"
        elif [[ "$TOTALMEMSWAP" -le '262144' && "$TOTALMEMSWAP" -ge '131073' ]]; then
          git config pack.windowMemory "50m"
          git config pack.packSizeLimit "250m"
        elif [[ "$TOTALMEMSWAP" -le '524288' && "$TOTALMEMSWAP" -ge '262145' ]]; then
          git config pack.windowMemory "125m"
          git config pack.packSizeLimit "250m"
        elif [[ "$TOTALMEMSWAP" -le '1048576' && "$TOTALMEMSWAP" -ge '524289' ]]; then
          git config pack.windowMemory "250m"
          git config pack.packSizeLimit "250m"
        elif [[ "$TOTALMEMSWAP" -le '2097152' && "$TOTALMEMSWAP" -ge '1048577' ]]; then
          git config pack.windowMemory "350m"
          git config pack.packSizeLimit "500m"
        elif [[ "$TOTALMEMSWAP" -le '4194304' && "$TOTALMEMSWAP" -ge '2097153' ]]; then
          git config pack.windowMemory "500m"
          git config pack.packSizeLimit "1000m"
        elif [[ "$TOTALMEMSWAP" -le '8388607' && "$TOTALMEMSWAP" -ge '4194305' ]]; then
          git config pack.windowMemory "1000m"
          git config pack.packSizeLimit "1000m"
        elif [[ "$TOTALMEMSWAP" -ge '8388608' ]]; then
          git config pack.windowMemory "1500m"
          git config pack.packSizeLimit "1000m"
        fi
        git gc --prune=now --aggressive
        touch git-agg
      fi
      echo
      git count-objects -v
    fi

    chmod +x centmin.sh
    
    echo
    cecho "list all available local branches" $boldyellow
    cecho " git branch -a" $boldgreen
    git branch -a
    
    echo
    cecho "list git log last commit" $boldyellow
    cecho " git log -a" $boldgreen
    git log -1 | sed -e 's|Author: George Liu <.*>|Author: George Liu <snipped>|g'
    
    echo
    cecho "to update centmin mod ${branchname} branch repo via git" $boldyellow
    cecho " cd /usr/local/src/centminmod" $boldgreen
    cecho " git stash" $boldgreen
    cecho " git pull" $boldgreen
    cecho " chmod +x centmin.sh" $boldgreen 

  echo
  cecho "####################### IMPORTANT #######################" $boldgreen
  cecho " To complete update " $boldyellow
  cecho "####################### IMPORTANT #######################" $boldgreen
  cecho " run:" $boldyellow
  echo
  echo "  cd /usr/local/src/centminmod"
  echo
  cecho " before invoking centmin.sh again" $boldyellow
  cecho "####################### IMPORTANT #######################" $boldgreen
  echo
  SPID=$$
  # echo "$SPID process id for centmin.sh ended"
  kill "$SPID"
  # re_sourcefiles
  # break
  break 2
  fi  
else
  echo
  echo "Error: you do not have git environment setup for github based updates"
  echo "       run submenu option 1 to setup github environment first"
  exit
fi
}

tidy_menulogs() {
# & older logs than 7 days deleted
if [ -d "${CENTMINLOGDIR}" ]; then
    # find "${CENTMINLOGDIR}" -type f -mtime +7 \( -name '*_git_envsetup.log' -o -name '*_git_updatecur_branch.log' -o -name "*_git_updatenew_branch.log" \) -print
    find "${CENTMINLOGDIR}" -type f -mtime +7 \( -name '*_git_envsetup.log' -o -name '*_git_updatecur_branch.log' -o -name "*_git_updatenew_branch.log" \) -delete
fi
}

updatersubmenu() {
  CM_DIRSIZE=$(df -P /usr/local/src/centminmod | awk '{print $4}' | tail -1 | awk '{print $1}')

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
  cecho "4). Exit" $boldgreen
  cecho "--------------------------------------------------------" $boldyellow

  read -ep "Enter option [ 1 - 4 ] " upmoption
  cecho "--------------------------------------------------------" $boldyellow

#########################################################

case "$upmoption" in
1)
UPDATE_MENU=1
centminlog
{
  gitenv_setup
  # re_sourcefiles
  # break
  break 2
} 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_git_envsetup.log"

  tidy_menulogs
;;
2)
UPDATE_MENU=2
centminlog
{
  gitenv_update
} 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_git_updatecur_branch.log"
  tidy_menulogs
;;
3)
UPDATE_MENU=3
centminlog
{
  gitenv_change
} 2>&1 | tee "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_git_updatenew_branch.log"
  tidy_menulogs
;;
4)
  UPDATE_MENU=4
  break 2
;;
esac
done

}