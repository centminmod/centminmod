#################
migrateact() {
echo ""
cecho "------------------------------------------------------" $boldyellow
cecho "vhost for $vhostname already exists" $boldwhite
cecho "/home/${USERNAME}/$vhostname already exists" $boldwhite
cecho "------------------------------------------------------" $boldyellow
echo ""

echo "
As at Centmin Mod v1.2.3-eva2000.04+ and higher, the
SFTP/SCP username/password feature is new . For this 
feature to work, the domain name structure changed.

If you want to migrate and existing domain name from
old to new SFTP/SCP user/pass structure following
instructions outlined below:
"
echo
echo "List of current Nginx VHOST configuration files:"

for d in $(cd /usr/local/nginx/conf/conf.d/; find . -type f -print | egrep -v '(ssl.conf|virtual.conf|.com.db)'); do echo $d ; done 

read -ep "Enter the domain name (minus .conf extension) you want to migrate: " migrate

echo "You have selected to migrate: $migrate"

echo "Checking to see if $migrate conf file is using old format"

if [[ -f "/usr/local/nginx/conf/conf.d/${migrate}.db" ]]; then
	echo "$migrate is already using the new format"
else
	echo "$migrate is using the old format"
fi
}

#################
createusract() {

if [[ "$ACCOUNTEXIST" = 'n' ]]; then

	#useradd -d /home/${USERNAME}/ -g nginx ${USERNAME}
	useradd -s /usr/bin/rssh -d /home/${USERNAME}/ -g nginx ${USERNAME}
	usermod -a -G rsshusers ${USERNAME}
	USERID=$(id ${USERNAME})
	UPASS=$(openssl rand 22 -base64)

	echo ""
	echo "${USERNAME}:${UPASS}" | chpasswd
	cecho "Your generated password: $UPASS" $boldyellow
	echo ""

cecho "------------------------------------------------------" $boldyellow
cecho "Configure user directory permissions..." $boldgreen
cecho "------------------------------------------------------" $boldyellow

echo "
	chmod 700 /home/${USERNAME}
	chown -R "${USERNAME}":nginx /home/${USERNAME}
	chown "${USERNAME}":nginx /home/${USERNAME}
	chmod g+s /home/${USERNAME}
"

	chmod 700 /home/${USERNAME}
	chown -R "${USERNAME}":nginx /home/${USERNAME}
	chown "${USERNAME}":nginx /home/${USERNAME}
	chmod g+s /home/${USERNAME}
fi
}

#################
usersetup() {

CURRENTSSHDPORT=$(echo ${SSH_CLIENT##* })

if [[ ! $(yum -q list installed | grep '^rssh') ]]; then
cecho "------------------------------------------------------" $boldyellow
cecho "Installing rssh..." $boldgreen
cecho "------------------------------------------------------" $boldyellow
	yum -q -y install rssh

cecho "------------------------------------------------------" $boldyellow
cecho "Configuring rssh..." $boldgreen
cecho "------------------------------------------------------" $boldyellow
sed -i 's/#allowscp/allowscp/g' /etc/rssh.conf
sed -i 's/#allowsftp/allowsftp/g' /etc/rssh.conf
echo "/etc/rssh.conf configured"
fi

cecho "------------------------------------------------------" $boldyellow
cecho "Setup SFTP/SCP username/pass for domain..." $boldgreen
cecho "------------------------------------------------------" $boldyellow
echo ""

read -ep "Do you want to continue ? [y/n] " usrcont

if [[ "$usrcont" == [[nN] ]]; then
	echo
	echo
	echo
	echo
	exit
fi

echo ""
read -ep "Enter SFTP/SCP username (alphanumeric only): " USERNAME

# checking and stripping non-alphanumeric characters
USERNAME=$(echo ${USERNAME//[^a-zA-Z0-9]/})

echo ""
cecho "You entered username: $USERNAME" $boldyellow

if [[ ! -d "/home/${USERNAME}/" ]]; then
	ACCOUNTEXIST='n'
else
	ACCOUNTEXIST='y'
fi


}

#################
creatediract() {

if [[ "$DIREXIST" = 'n' ]]; then

# Checking Permissions, making directories, example index.html
mkdir -p /home/${USERNAME}/$vhostname
chown "${USERNAME}":nginx /home/${USERNAME}/$vhostname
chown -R "${USERNAME}":nginx /home/${USERNAME}/$vhostname
chmod g+s /home/${USERNAME}/$vhostname

mkdir -p /home/${USERNAME}/$vhostname/{public,private,log,backup}
chown "${USERNAME}":nginx /home/${USERNAME}/$vhostname/{public,private,log,backup}
chown -R "${USERNAME}":nginx /home/${USERNAME}/$vhostname/{public,private,log,backup}
chmod g+s /home/${USERNAME}/$vhostname/{public,private,log,backup}

cat > "/home/${USERNAME}/$vhostname/public/index.html" <<END
<html>
<head>
<title>$vhostname</title>
</head>
<body>
Welcome to $vhostname
</body>
</html>
END

    cp -R $CUR_DIR/htdocs/custom_errorpages/* /home/${USERNAME}/$vhostname/public

chown "${USERNAME}":nginx "/home/${USERNAME}/$vhostname"
chown -R "${USERNAME}":nginx "/home/${USERNAME}/$vhostname"
find "/home/${USERNAME}/$vhostname" -type d -exec chmod g+s {} \;

# Setup user domain mapping file
echo "Created: $(date)" > /usr/local/nginx/conf/conf.d/$vhostname.db

# Setting up Nginx mapping
cat > "/usr/local/nginx/conf/conf.d/$vhostname.conf" <<END
server {
  server_name $vhostname www.$vhostname;

# ngx_pagespeed & ngx_pagespeed handler
#include /usr/local/nginx/conf/pagespeed.conf;
#include /usr/local/nginx/conf/pagespeedhandler.conf;
#include /usr/local/nginx/conf/pagespeedstatslog.conf;

  # limit_conn limit_per_ip 16;
  # ssi  on;

  access_log /home/${USERNAME}/$vhostname/log/access.log combined buffer=32k;
  error_log /home/${USERNAME}/$vhostname/log/error.log;

  root /home/${USERNAME}/$vhostname/public;

  location / {

# block common exploits, sql injections etc
#include /usr/local/nginx/conf/block.conf;

  # Enables directory listings when index file not found
  #autoindex  on;

  # Shows file listing times as local time
  #autoindex_localtime on;

  # Enable for vBulletin usage WITHOUT vbSEO installed
  #try_files		$uri $uri/ /index.php;

  }

  include /usr/local/nginx/conf/staticfiles.conf;
  include /usr/local/nginx/conf/php.conf;
  include /usr/local/nginx/conf/drop.conf;
  #include /usr/local/nginx/conf/errorpage.conf;
}
END

echo 
cecho "------------------------------------------------------" $boldyellow
service nginx reload

	echo
	echo
	echo
	echo

echo 
cecho "####################################" $boldgreen
cecho "vhost for $vhostname created successfully" $boldwhite
cecho "vhost conf file for $vhostname created: /usr/local/nginx/conf/conf.d/$vhostname.conf" $boldwhite
cecho "upload files to /home/${USERNAME}/$vhostname/public" $boldwhite
cecho "vhost log files directory is /home/${USERNAME}/$vhostname/log" $boldwhite
cecho "------------------------------------------------------" $boldyellow
cecho "SFTP/SCP username: ${USERNAME}" $boldyellow
cecho "SFTP/SCP password: ${UPASS}" $boldyellow
cecho "SFTP/SCP port (current): ${CURRENTSSHDPORT}" $boldyellow
cecho "id ${USERNAME}: $USERID" $boldyellow
cecho "####################################" $boldgreen

#echo "" >> /usr/local/nginx/conf/conf.d/$vhostname.db
echo "${USERNAME} $vhostname" >> /usr/local/nginx/conf/conf.d/$vhostname.db
echo "" >> /usr/local/nginx/conf/conf.d/$vhostname.db

echo
cecho "Current vhost listing at: /usr/local/nginx/conf/conf.d/" $boldwhite

ls -Alhrt /usr/local/nginx/conf/conf.d/ | awk '{ printf "%-4s%-4s%-8s%-6s %s\n", $6, $7, $8, $5, $9 }'

cecho "------------------------------------------------------" $boldyellow

	echo
	echo
	echo

else

echo ""
cecho "------------------------------------------------------" $boldyellow
cecho "vhost for $vhostname already exists" $boldwhite
cecho "/home/${USERNAME}/$vhostname already exists" $boldwhite
cecho "------------------------------------------------------" $boldyellow
echo ""

	echo
	echo
	echo
fi

}

#################
function funct_nginxaddvhost {

echo ""
cecho "---------------------------------------------" $boldyellow

usersetup

echo ""
read -ep "Enter vhost domain name you want to add (without www. prefix): " vhostname

echo ""

if [ ! -d /home/${USERNAME}/$vhostname ]; then
	DIREXIST='n'
else
	DIREXIST='y'
fi

if [ ! -f /usr/local/nginx/conf/conf.d/${vhostname}.conf ]; then
	VHOSTCONFEXIST='n'
else
	VHOSTCONFEXIST='y'
fi

if [[ "$ACCOUNTEXIST" = 'n' && "$DIREXIST" = 'n' && "$VHOSTCONFEXIST" = 'n' ]]; then
	createusract
	creatediract
else
	cecho "Either one of the following already exist:
* Username: $USERNAME or 
* Domain: $vhostname or
* Nginx VHOST: /usr/local/nginx/conf/conf.d/${vhostname}.conf
	" $boldyellow
fi

}