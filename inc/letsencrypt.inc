python_setup() {
	if [ -f /usr/local/src/centminmod/addons/python27_install.sh ]; then
		if [[ "$CENTOS_SIX" = '6' && ! -f /usr/bin/python2.7 ]]; then
			/usr/local/src/centminmod/addons/python27_install.sh install
		fi
	fi
}

leclientsetup() {
	# setup letsencrypt client and virtualenv
	# https://community.centminmod.com/posts/19914/
	echo
	echo "installing or updating letsencrypt client"
	echo
	python_setup
	echo
	mkdir -p /root/tools
	cd /root/tools
	rm -rf /root/tools/letsencrypt
	git clone https://github.com/letsencrypt/letsencrypt
	cd letsencrypt
	if [[ "$CENTOS_SIX" = '6' && -f /usr/bin/python2.7 ]]; then
		sed -i "s|--python python2|--python python2.7|" letsencrypt-auto
	fi
	# staging endpoint
	LE_SERVER='https://acme-staging.api.letsencrypt.org/directory'
	# live and beta invitee trusted cert endpoint
	# LE_SERVER='https://acme-v01.api.letsencrypt.org/directory'
	if [ -f /root/.local/share/letsencrypt/bin/letsencrypt ]; then
		/root/.local/share/letsencrypt/bin/letsencrypt --agree-dev-preview --server $LE_SERVER
	else
		echo "/root/.local/share/letsencrypt/bin/letsencrypt not found"
	fi

	if [ ! -f /etc/letsencrypt/webroot.ini ]; then
	echo "setup general /etc/letsencrypt/webroot.ini letsencrypt config file"
cat > "/etc/letsencrypt/webroot.ini" <<EOF
# webroot.ini general config ini

rsa-key-size = 2048

# Always use the staging/testing server
server = https://acme-staging.api.letsencrypt.org/directory

# for beta invitees
# server = https://acme-v01.api.letsencrypt.org/directory

# Uncomment and update to register with the specified e-mail address
email = foo@example.com

# Uncomment to use a text interface instead of ncurses
text = True
agree-tos = True
agree-dev-preview = True
renew-by-default = True

authenticator = webroot
EOF
	fi

	if [[ "$(grep 'foo@example.com' /etc/letsencrypt/webroot.ini)" ]]; then
		echo
		echo "Registering an account with Letsencrypt"
		echo "You only do this once, so that Letsencrypt can notify &"
		echo "contact you via email regarding your SSL certificates"
		read -ep "Enter your email address to setup Letsencrypt account: " letemail
		sed -i "s|foo@example.com|$letemail|" /etc/letsencrypt/webroot.ini
		echo
	fi

}