#!/bin/bash
#################################################################
# for centminmod.com LEMP stack environments
# auto generate and convert detected .htaccess file directory
# paths to nginx deny all equivalents only when .htaccess file
# contains 'Deny from all' text and generate a per nginx vhost
# auto protect directory at /usr/local/nginx/conf/autoprotect/${domain}
# with /usr/local/nginx/conf/autoprotect/${domain}/autoprotect-${domain}.conf
# include file which you include into your nginx vhost above the
# root / location match i.e.
# include /usr/local/nginx/conf/autoprotect/${domain}/autoprotect-${domain}.conf;
#################################################################
DT=$(date +"%d%m%y-%H%M%S")
SCRIPTDIR="$(cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd)"
BASEDIR=$(dirname $SCRIPTDIR)

DEBUG='n'
TOPLEVEL_DIR='/home/nginx/domains'

genprotect() {
  # need to restart to ensure all existing or recently 403 denied directory settings
  # are detected
  /usr/bin/ngxrestart >/dev/null 2>&1
  sleep 2

	# generate /usr/local/nginx/conf/autoprotect.conf include file
	# to be placed in individual nginx vhosts
for domain in $(ls $TOPLEVEL_DIR); do
	if [ ! -d "/usr/local/nginx/conf/autoprotect/${domain}" ]; then
  	mkdir -p "/usr/local/nginx/conf/autoprotect/${domain}"
	fi
	HTPATHS=$(find "${TOPLEVEL_DIR}/${domain}" -name ".htaccess" -print0 | xargs -0 echo)
	declare -a arrays
	arrays=(${HTPATHS})
  for d in "${arrays[@]}"; do
    if [[ "$(echo "$d" | grep "${domain}/public/")" ]]; then
      # get directory path which contains .htaccess file
      PROTECTDIR=$(dirname "$d");
      # get the web url path for the .htaccess file
      URL_WEB=$(echo "$PROTECTDIR" |sed -e 's|\/home\/nginx\/domains\/||' -e 's|\/public||')
      # check if 'Deny from all' exists in .htaccess and only generate nginx deny all location
      # matches if .htaccess file has 'Deny from all' but also give end user option to bypass
      # autoprotect.sh script and NOT create a nginx deny all location match by manually creating
      # a .autoprotect-bypass file within the directory you want to bypass and exclude from autoprotect.sh
      if [ -f "${PROTECTDIR}/.htaccess" ]; then
        if [[ "$(cat "${PROTECTDIR}/.htaccess" | grep -v "\#" | grep -i 'Deny from all' >/dev/null 2>&1; echo $?)" = '0' && ! -f "${PROTECTDIR}/.autoprotect-bypass" ]]; then
          # check the web url for .htaccess directory path to see if it's already deny all / 403 protected
          # only generate nginx deny all rules for .htaccess directory paths which are not already returning
          # 403 permission denied http status codes (also include check for 404)
          # 
          # if [[ "$(curl -sI ${URL_WEB}/ | grep 'HTTP\/' | egrep -o '403|404' >/dev/null 2>&1; echo $?)" != '0' ]]; then
            PROTECTDIR_PATH=$(echo "$PROTECTDIR" |sed -e "s|\/home\/nginx\/domains\/${domain}\/public||")
            echo -e "# $PROTECTDIR\nlocation ~* ^$PROTECTDIR_PATH/ { deny all; }"
          # fi
        fi
      fi
    fi # grep public only
  done > "/usr/local/nginx/conf/autoprotect/${domain}/autoprotect-${domain}.conf"
    echo "generated nginx include file: /usr/local/nginx/conf/autoprotect/${domain}/autoprotect-${domain}.conf"
done

if [[ "$DEBUG" = [yY] ]]; then
  echo
  ls -lahR /usr/local/nginx/conf/autoprotect/
  echo
else
  echo
  echo "autoprotect.sh run completed..."
  echo
fi

# only trigger nginx restart service when there are new differences detected
# in current and previous autoprotect include conf files
if [ -f /etc/init.d/nginx restart ]; then
		# /etc/init.d/nginx restart >/dev/null 2>&1
		sleep 2
		/etc/init.d/nginx restart
		rm -rf /usr/local/nginx/conf/autoprotect/status-restart
fi

}

genprotect