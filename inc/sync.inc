dropbox_send() {
  file="$1"
  filename=$(basename $file)
  filename_only="${filename%.*}"
  file_ext="${filename_only##*.}"
  if [[ "$DROPBOX_SEND_DEBUG" = [yY] ]]; then
    echo "$file"
    echo "$filename"
    # echo "$filename_only"
    # echo "$file_ext"
    echo
  fi
  if [ ! -f /usr/local/bin/dbsend ]; then
    curl -4sL https://gist.githubusercontent.com/centminmod/dca86d219b5bb98dcf26c602c548487e/raw/dbsend.sh -o /usr/local/bin/dbsend
    chmod +x /usr/local/bin/dbsend
  elif [ -f /usr/local/bin/dbsend ]; then
    curl -4sL https://gist.githubusercontent.com/centminmod/dca86d219b5bb98dcf26c602c548487e/raw/dbsend.sh -o /usr/local/bin/dbsend
    chmod +x /usr/local/bin/dbsend
  fi
  if [[ "$DROPBOX_SEND" = [yY] && -f /usr/sbin/rclone && "$(/usr/sbin/rclone listremotes | grep -o dropbox1)" = 'dropbox1' && "$RCLONE_ENABLE" = [yY] ]]; then
    dropbox_wrkdir=$PWD
    cd "${CENTMINLOGDIR}"
    if [ -f "$filename" ]; then
      echo "/usr/sbin/rclone copy $filename dropbox1:"
      /usr/sbin/rclone --fast-list --dropbox-batch-mode=off --dropbox-batch-size 10 --dropbox-batch-timeout 10s copy $filename dropbox1:
      echo
    fi
    cd "$dropbox_wrkdir"
  elif [[ "$DROPBOX_SEND" = [yY] && -f /usr/local/bin/dbsend && "$DROPBOX_EMAIL_TO" ]]; then
    if [ -f "$file" ]; then
      sleep 2.5
      echo "/usr/local/bin/dbsend send $file"
      /usr/local/bin/dbsend send "$file"
      echo
    fi
  fi
}

dropbox_generate() {
  DT="$1"
  CONFIG_STAGE="$2"
  if [[ "$CONFIG_STAGE" = 'configure-error' ]]; then
    DROPBOX_LABEL='-configure-stage-error'
  elif [[ "$CONFIG_STAGE" = 'make-error' ]]; then
    DROPBOX_LABEL='-make-stage-error'
  elif [[ "$CONFIG_STAGE" = 'pcre-install-config-err' ]]; then
    DROPBOX_LABEL_NGINX_PCRE='-pcre-install-config-err'
  elif [[ "$CONFIG_STAGE" = 'pcre-install-make-err' ]]; then
    DROPBOX_LABEL_NGINX_PCRE='-pcre-install-make-err'
  elif [[ "$CONFIG_STAGE" = 'pcre-install-cmakeinstall-err' ]]; then
    DROPBOX_LABEL_NGINX_PCRE='-pcre-install-cmakeinstall-err'
  elif [[ "$CONFIG_STAGE" = 'pcre2-install-config-err' ]]; then
    DROPBOX_LABEL_NGINX_PCRE='-pcre2-install-config-err'
  elif [[ "$CONFIG_STAGE" = 'pcre2-install-make-err' ]]; then
    DROPBOX_LABEL_NGINX_PCRE='-pcre2-install-make-err'
  elif [[ "$CONFIG_STAGE" = 'pcre2-install-cmakeinstall-err' ]]; then
    DROPBOX_LABEL_NGINX_PCRE='-pcre2-install-cmakeinstall-err'
  else
    DROPBOX_LABEL=""
  fi
  if [[ "$DROPBOX_SEND" = [yY] ]]; then
    echo
    echo "Send Nginx Upgrade Logs To Dropbox Account"
    echo
    # compressed logs to send via Dropbox sync if enabled
    if [[ "$DROPBOX_SEND_DEBUG" = [yY] ]]; then
      find $CENTMINLOGDIR -type f \( -name "*nginx_pcre_checks*" -o -name "*nginx_upgrade*" \) | grep $DT | xargs pigz -9kv
    else
      find $CENTMINLOGDIR -type f \( -name "*nginx_pcre_checks*" -o -name "*nginx_upgrade*" \) | grep $DT | xargs pigz -9kq
    fi
    # send compressed logs
    pcrecheck=$(ls -rt "${CENTMINLOGDIR}" | grep $DT | grep 'nginx_pcre_checks' | grep '\.gz')
    # pcrecheck=$(echo "$pcrecheck" | sed -e "s|nginx_pcre_checks|nginx_pcre_checks${DROPBOX_LABEL}|")
    if [[ "$DROPBOX_SEND_DEBUG" = [yY] ]]; then
      echo "----------------------------------------------------"
      echo "dropbox_send ${CENTMINLOGDIR}/$pcrecheck"
      echo "----------------------------------------------------"
    fi
    dropbox_send "${CENTMINLOGDIR}/$pcrecheck"
    ngxupgrade=$(ls -rt "${CENTMINLOGDIR}" | grep $DT | grep 'nginx_upgrade' | grep '\.gz')
    ngxupgrade_filename=$(echo $ngxupgrade | sed -e 's|.log.gz||')
    ngxupgrade_newfilename=$(echo "$ngxupgrade_filename" | sed -e "s|centminmod|private-cm|")
    ngxupgrade_newfile="${ngxupgrade_newfilename}.log.gz"
    mv "${CENTMINLOGDIR}/${ngxupgrade}" "${CENTMINLOGDIR}/$ngxupgrade_newfile"
    # ngxupgrade=$(echo "$ngxupgrade" | sed -e "s|nginx_upgrade|nginx_upgrade${DROPBOX_LABEL}|")
    
    if [[ "$DROPBOX_SEND_DEBUG" = [yY] ]]; then
      echo "----------------------------------------------------"
      echo "dropbox_send ${CENTMINLOGDIR}/$ngxupgrade_newfile"
      echo "----------------------------------------------------"
    fi
    dropbox_send "${CENTMINLOGDIR}/$ngxupgrade_newfile"
    # generate sanitized nginx_upgrade log renamed as pcre2log.log for PCRE2 tests
    egrep -v '/usr/local/nginx/conf/conf.d|Copyright (C)|This is free software|MERCHANTABILITY' ${CENTMINLOGDIR}/$(ls -rt ${CENTMINLOGDIR}/ | grep nginx_upgrade | grep $DT | tail -1) | sed -e "s|$HOSTNAME|hostname|g" > "${CENTMINLOGDIR}/public-nginx-pcre2log-$DT.log"
    create_pcretwo_log_err=$?
    # compress santized nginx_upgrade log renamed as public-nginx-pcre2log-$DT.log
    pigz -9kfq "${CENTMINLOGDIR}/public-nginx-pcre2log-$DT.log"
    pigz_pcretwo_compress_err=$?
    if [[ "$NGX_LDMOLD" = [yY] ]]; then
      mv "${CENTMINLOGDIR}/public-nginx-pcre2log-$DT.log.gz" "${CENTMINLOGDIR}/public-nginx-pcre2log-$DT-mold.log.gz"
      pcretwo_log="public-nginx-pcre2log-$DT-mold.log.gz"
    else
      pcretwo_log="public-nginx-pcre2log-$DT.log.gz"
    fi
    # send public-nginx-pcre2log-$DT.log.gz compressed version
    if [[ "$DROPBOX_SEND_DEBUG" = [yY] ]]; then
      echo "----------------------------------------------------"
      echo "dropbox_send ${CENTMINLOGDIR}/${pcretwo_log}"
      echo "----------------------------------------------------"
    fi
    dropbox_send "${CENTMINLOGDIR}/${pcretwo_log}"
    if [ -f "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_pcre-nginx_install.log" ]; then
      pigz -9kfq "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_pcre-nginx_install.log"
      pcrenginx_install_log="centminmod_${SCRIPT_VERSION}_${DT}_pcre-nginx_install.log.gz"
      # pcrenginx_install_log=$(echo "$pcrenginx_install_log" | sed -e "s|pcre-nginx_install|pcre-nginx_install${DROPBOX_LABEL_NGINX_PCRE}|")
      # send centminmod_${SCRIPT_VERSION}_${DT}_pcre-nginx_install.log.gz compressed version
      if [[ "$DROPBOX_SEND_DEBUG" = [yY] ]]; then
        echo "----------------------------------------------------"
        echo "dropbox_send ${CENTMINLOGDIR}/${pcrenginx_install_log}"
        echo "----------------------------------------------------"
      fi
      dropbox_send "${CENTMINLOGDIR}/${pcrenginx_install_log}"
    fi
    if [ -f "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_pcre2_install.log" ]; then
      pigz -9kfq "${CENTMINLOGDIR}/centminmod_${SCRIPT_VERSION}_${DT}_pcre2_install.log"
      pcretwo_nginx_install_log="centminmod_${SCRIPT_VERSION}_${DT}_pcre2_install.log.gz"
      # pcretwo_nginx_install_log=$(echo "$pcretwo_nginx_install_log" | sed -e "s|pcre2_install|pcre2_install${DROPBOX_LABEL_NGINX_PCRE}|")
      # send centminmod_${SCRIPT_VERSION}_${DT}_pcre2_install.log.gz compressed version
      if [[ "$DROPBOX_SEND_DEBUG" = [yY] ]]; then
        echo "----------------------------------------------------"
        echo "dropbox_send ${CENTMINLOGDIR}/${pcretwo_nginx_install_log}"
        echo "----------------------------------------------------"
      fi
      dropbox_send "${CENTMINLOGDIR}/${pcretwo_nginx_install_log}"
    fi
    if [ -f "${CENTMINLOGDIR}/nginx-configure_autoconf.err.${DT}.log" ]; then
      pigz -9kfq "${CENTMINLOGDIR}/nginx-configure_autoconf.err.${DT}.log"
      ngxautoconf_config_log="nginx-configure_autoconf.err.${DT}.log.gz"
      # send compressed version
      if [[ "$DROPBOX_SEND_DEBUG" = [yY] ]]; then
        echo "----------------------------------------------------"
        echo "dropbox_send ${CENTMINLOGDIR}/${ngxautoconf_config_log}"
        echo "----------------------------------------------------"
      fi
      dropbox_send "${CENTMINLOGDIR}/${ngxautoconf_config_log}"
    fi
    if [ -f "${CENTMINLOGDIR}/nginx-make_autoconf.err.${DT}.log" ]; then
      pigz -9kfq "${CENTMINLOGDIR}/nginx-make_autoconf.err.${DT}.log"
      ngxautoconf_make_log="nginx-make_autoconf.err.${DT}.log.gz"
      # send compressed version
      if [[ "$DROPBOX_SEND_DEBUG" = [yY] ]]; then
        echo "----------------------------------------------------"
        echo "dropbox_send ${CENTMINLOGDIR}/${ngxautoconf_make_log}"
        echo "----------------------------------------------------"
      fi
      dropbox_send "${CENTMINLOGDIR}/${ngxautoconf_make_log}"
    fi
    if [ -f "${CENTMINLOGDIR}/nginx_autoconf.err.${DT}.log" ]; then
      pigz -9kfq "${CENTMINLOGDIR}/nginx_autoconf.err.${DT}.log"
      ngxautoconf_install_log="nginx_autoconf.err.${DT}.log.gz"
      # send compressed version
      if [[ "$DROPBOX_SEND_DEBUG" = [yY] ]]; then
        echo "----------------------------------------------------"
        echo "dropbox_send ${CENTMINLOGDIR}/${ngxautoconf_install_log}"
        echo "----------------------------------------------------"
      fi
      dropbox_send "${CENTMINLOGDIR}/${ngxautoconf_install_log}"
    fi
    if [ -f "${CENTMINLOGDIR}/nginx_pcre2_download_${DT}.log" ]; then
      pigz -9kfq "${CENTMINLOGDIR}/nginx_pcre2_download_${DT}.log"
      pcre_two_download_log="nginx_pcre2_download_${DT}.log.gz"
      # send compressed version
      if [[ "$DROPBOX_SEND_DEBUG" = [yY] ]]; then
        echo "----------------------------------------------------"
        echo "dropbox_send ${CENTMINLOGDIR}/${pcre_two_download_log}"
        echo "----------------------------------------------------"
      fi
      dropbox_send "${CENTMINLOGDIR}/${pcre_two_download_log}"
    fi
    if [[ -f "${CENTMINLOGDIR}/nginx_pcre_download_${DT}.log" && "$NGINX_PCRE_TWO" != [yY] ]]; then
      pigz -9kfq "${CENTMINLOGDIR}/nginx_pcre_download_${DT}.log"
      pcre_download_log="nginx_pcre_download_${DT}.log.gz"
      # send compressed version
      if [[ "$DROPBOX_SEND_DEBUG" = [yY] ]]; then
        echo "----------------------------------------------------"
        echo "dropbox_send ${CENTMINLOGDIR}/${pcre_download_log}"
        echo "----------------------------------------------------"
      fi
      dropbox_send "${CENTMINLOGDIR}/${pcre_download_log}"
    fi
  fi
}