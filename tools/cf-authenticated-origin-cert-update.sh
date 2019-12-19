#!/bin/bash
###################################################
# tool to update Cloudflare Authenticated Origin Pulls
# certificates
# https://community.centminmod.com/threads/13847/
###################################################
DT=$(date +"%d%m%y-%H%M%S")
cf_auth_origin_cert_dir='/usr/local/nginx/conf/ssl/cloudflare'
cf_auth_origin_cert='https://support.cloudflare.com/hc/en-us/article_attachments/360044928032/origin-pull-ca.pem'

cfauthorigin_cronsetup() {
  if [[ -z "$(crontab -l 2>&1 | grep '\/usr\/local\/src\/centminmod\/tools\/cf-authenticated-origin-cert-update.sh')" && -f "/usr/local/src/centminmod/tools/cf-authenticated-origin-cert-update.sh" ]]; then
    mkdir -p /etc/centminmod/cronjobs/
    crontab -l > /etc/centminmod/cronjobs/cronjoblist-before-cf-auth-origin-pull-update.txt
    sed -i '/cf-authenticated-origin-cert-update.sh/d' /etc/centminmod/cronjobs/cronjoblist-before-cf-auth-origin-pull-update.txt
    echo "0 0 * * 5 /usr/local/src/centminmod/tools/cf-authenticated-origin-cert-update.sh update >/dev/null 2>&1" >> /etc/centminmod/cronjobs/cronjoblist-before-cf-auth-origin-pull-update.txt
    crontab /etc/centminmod/cronjobs/cronjoblist-before-cf-auth-origin-pull-update.txt
  fi
}

cforigin() {
  mode=$1
  list_origincrt_domains=$(find ${cf_auth_origin_cert_dir} -type f -name 'origin.crt' -exec dirname {} \; 2>&1 | sed -e "s|${cf_auth_origin_cert_dir}/||g")
  if [ "$list_origincrt_domains" ]; then
    for d in $list_origincrt_domains; do
      expiry=$(openssl x509 -enddate -noout -in "${cf_auth_origin_cert_dir}/${d}/origin.crt" | cut -d'=' -f2 | awk '{print $2 " " $1 " " $4}')
      epochExpirydate=$(date -d"${expiry}" +%s)
      epochToday=$(date +%s)
      secondsToExpire=$(echo ${epochExpirydate} - ${epochToday} | bc)
      daysToExpire=$(echo "${secondsToExpire} / 60 / 60 / 24" | bc)
      echo -e "------------------------------\n$d cloudflare authenticated origin cert expires in $daysToExpire days on $expiry"
      if [[ "$daysToExpire" -le '180' ]]; then
        if [[ "$mode" = 'check' ]]; then
          echo "at ${cf_auth_origin_cert_dir}/${d}/origin.crt"
        elif [[ "$mode" = 'update' ]]; then
          echo "updating $d cloudflare authenticated origin cert"
          echo "at ${cf_auth_origin_cert_dir}/${d}/origin.crt"
          cp -a "${cf_auth_origin_cert_dir}/${d}/origin.crt" "${cf_auth_origin_cert_dir}/${d}/origin.crt-backup"
          wget -4 -q -O "${cf_auth_origin_cert_dir}/${d}/origin.crt" "$cf_auth_origin_cert"
          err=$?
          if [[ "$err" -eq '0' ]]; then
            echo "succesfully updated ${cf_auth_origin_cert_dir}/${d}/origin.crt"
            rm -f "${cf_auth_origin_cert_dir}/${d}/origin.crt-backup"
            service nginx reload >/dev/null 2>&1
            expiry=$(openssl x509 -enddate -noout -in "${cf_auth_origin_cert_dir}/${d}/origin.crt" | cut -d'=' -f2 | awk '{print $2 " " $1 " " $4}')
            epochExpirydate=$(date -d"${expiry}" +%s)
            epochToday=$(date +%s)
            secondsToExpire=$(echo ${epochExpirydate} - ${epochToday} | bc)
            daysToExpire=$(echo "${secondsToExpire} / 60 / 60 / 24" | bc)
            echo -e "$d cloudflare authenticated origin cert now expires in $daysToExpire days on $expiry"
          else
            echo "failed to update ${cf_auth_origin_cert_dir}/${d}/origin.crt"
            echo "restoring previous backup"
            \cp -af "${cf_auth_origin_cert_dir}/${d}/origin.crt-backup" "${cf_auth_origin_cert_dir}/${d}/origin.crt"
            service nginx reload >/dev/null 2>&1
          fi
        fi
      fi
    done
  else
    echo "no Cloudflare Authenticated Origin Pull Certs to update"
    echo "at ${cf_auth_origin_cert_dir}"
  fi
}

case "$1" in
  update )
    cforigin update
    cfauthorigin_cronsetup
    ;;
  check )
    cforigin check
    cfauthorigin_cronsetup
    ;;
  * )
    echo "$0 update"
    echo "$0 check"
    ;;
esac