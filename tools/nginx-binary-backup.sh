#!/bin/bash
#########################################################
# backup nginx binary and modules
# written by George Liu (eva2000) https://centminmod.com
#########################################################
# variables
#############
DT=$(date +"%d%m%y-%H%M%S")

DIR_TMP='/svr-setup'
CENTMINLOGDIR='/root/centminlogs'
SCRIPT_DIR=$(readlink -f $(dirname ${BASH_SOURCE[0]}))
CONFIGSCANBASE='/etc/centminmod'

NGINXBIN_BACKUPDIR='/home/backup-nginxbin'
NGINXBIN_VER=$(nginx -v 2>&1 | awk '{print $3}' | awk -F '/' '{print $2}')
NGINXBIN_COMPILER=$(nginx -V 2>&1 | awk '/built by/ {print $3}' | awk '{print tolower($0)}')
NGINXBIN_CRYPTO=$(nginx -V 2>&1 | awk '/built with/ {print $3"-"$4}' | awk '{print tolower($0)}')
NGINXBIN_PATH='/usr/local/sbin/nginx'
NGINXTOP_DIR='/usr/local/nginx'
NGINXBIN_MODULESDIR="$NGINXTOP_DIR/modules"
NGINXMODULE_INCLUDENAME='dynamic-modules.conf'
NGINXMODULE_INCLUDED_INCLUDENAME='dynamic-modules-includes.conf'
NGINXMODULE_INCLUDE="$NGINXTOP_DIR/conf/$NGINXMODULE_INCLUDENAME"
NGINXMODULE_INCLUDED_INCLUDE="$NGINXTOP_DIR/conf/$NGINXMODULE_INCLUDED_INCLUDENAME"
#####################################################
if [[ ! -d "$NGINXBIN_BACKUPDIR" ]]; then
  mkdir -p "$NGINXBIN_BACKUPDIR"
fi

if [[ "$NGINXBIN_COMPILER" = 'gcc' ]]; then
  NGINXBIN_COMPILERNAME=$(nginx -V 2>&1 | awk '/built by/ {print $3"-"$4"-"$5}')
elif [[ "$NGINXBIN_COMPILER" = 'clang' ]]; then
  NGINXBIN_COMPILERNAME=$(nginx -V 2>&1 | awk '/built by/ {print $3"-"$4"-"$5}' | sed -e 's|(||g' -e 's|)||g' -e 's|\/|-|g')
fi

bin_backup() {
  verbose=$1
  DDT=$(date +"%d%m%y-%H%M%S")
  backup_tag="${NGINXBIN_VER}-${NGINXBIN_COMPILERNAME}-${NGINXBIN_CRYPTO}-${DDT}"
  if [ ! -d "${NGINXBIN_BACKUPDIR}/${backup_tag}" ]; then
    echo "--------------------------------------------------------"
    echo "backup current Nginx binary and dynamic modules"
    echo "--------------------------------------------------------"
    mkdir -p "${NGINXBIN_BACKUPDIR}/${backup_tag}/bin"
    cp -af "$NGINXBIN_PATH" "${NGINXBIN_BACKUPDIR}/${backup_tag}/bin"
    cp -af "$NGINXBIN_MODULESDIR" "${NGINXBIN_BACKUPDIR}/${backup_tag}"
    cp -af "$NGINXMODULE_INCLUDE" "${NGINXBIN_BACKUPDIR}/${backup_tag}"
    cp -af "$NGINXMODULE_INCLUDED_INCLUDE" "${NGINXBIN_BACKUPDIR}/${backup_tag}"
    if [[ "$verbose" != 'quiet' ]]; then
    echo "--------------------------------------------------------"
      ls -lahR "${NGINXBIN_BACKUPDIR}/${backup_tag}"
    fi
    echo "--------------------------------------------------------"
    echo "backup created at ${NGINXBIN_BACKUPDIR}/${backup_tag}"
    echo "--------------------------------------------------------"
  fi
}

bin_list() {
  if [ -d "${NGINXBIN_BACKUPDIR}" ]; then
    echo "--------------------------------------------------------"
    echo "Listing of available Nginx binary/module backups"
    echo "--------------------------------------------------------"
    find "${NGINXBIN_BACKUPDIR}" -mindepth 1 -maxdepth 1 -type d
    echo "--------------------------------------------------------"
  fi
}

bin_restore() {
    if [ "$1" ]; then
      backup_path="$1"
    fi
    echo "--------------------------------------------------------"
    echo "Restore Nginx binary/module from backups"
    echo "--------------------------------------------------------"
    if [ ! -d "$backup_path" ]; then
      bin_list
      echo
      echo "--------------------------------------------------------"
      read -ep "Enter full path of backup to restore: " backup_path
      echo
      echo "You entered $backup_path"
      echo
      read -ep "Is this correct ? [y/n] " is_correct
    elif [[ -d "$backup_path" ]]; then
      is_correct='y'
    fi # unattended
      if [[ "$is_correct" = [yY] ]]; then
        if [ -d "${backup_path}" ]; then
          # backup before restore
          bin_backup quiet
          echo
          echo "restoring..."
          echo
          if [ -f "${backup_path}/bin/nginx" ]; then
            echo "cp -af ${backup_path}/bin/nginx $NGINXBIN_PATH"
            cp -af "${backup_path}/bin/nginx" "$NGINXBIN_PATH"
            ls -lah "$NGINXBIN_PATH"
          fi
          if [ -d "${backup_path}/modules" ]; then
            echo
            rm -rf "$NGINXBIN_MODULESDIR"
            echo "cp -af ${backup_path}/modules $NGINXTOP_DIR"
            cp -af "${backup_path}/modules" "$NGINXTOP_DIR"
            ls -lah "$NGINXTOP_DIR/modules"
          fi
          if [ -f "${backup_path}/${NGINXMODULE_INCLUDENAME}" ]; then
            echo
            echo "cp -af ${backup_path}/${NGINXMODULE_INCLUDENAME} $NGINXMODULE_INCLUDE"
            cp -af "${backup_path}/${NGINXMODULE_INCLUDENAME}" "$NGINXMODULE_INCLUDE"
            ls -lah "$NGINXMODULE_INCLUDE"
          fi
          if [ -f "${backup_path}/${NGINXMODULE_INCLUDED_INCLUDENAME}" ]; then
            echo
            echo "cp -af ${backup_path}/${NGINXMODULE_INCLUDED_INCLUDENAME} $NGINXMODULE_INCLUDED_INCLUDE"
            cp -af "${backup_path}/${NGINXMODULE_INCLUDED_INCLUDENAME}" "$NGINXMODULE_INCLUDED_INCLUDE"
            ls -lah "$NGINXMODULE_INCLUDED_INCLUDE"
          fi
          echo "--------------------------------------------------------"
          echo "Restored Nginx binary/module from"
          echo "$backup_path"
          echo "--------------------------------------------------------"
          echo "nginx -t"
          nginx -t
          echo
          echo "ngxreload"
          ngxreload
        fi
      fi
    echo "--------------------------------------------------------"
}

#########################################################
case $1 in
  backup )
    bin_backup
    ;;
  list )
    bin_list
    ;;
  restore )
    bin_restore $2
    ;;
  pattern )
    ;;
  pattern )
    ;;
  * )
    echo
    echo "$0 {backup|list|restore}"
    ;;
esac
exit
