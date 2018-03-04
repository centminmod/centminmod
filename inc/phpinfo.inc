phpiadmin() {
    if [ -f /usr/local/nginx/html/phpi.php ]; then
        # Rename phpi.php filename
        NPHP="gateway"
        PHPIUSER=$(echo "phpiadmin")
        PHPIPASS=$(openssl rand 19 -base64 | tr -dc 'a-zA-Z0-9')
        echo ""
        mv /usr/local/nginx/html/phpi.php "/usr/local/nginx/html/${NPHP}-phpi.php"
        # cecho "Insert Username/Password into /usr/local/nginx/html/${NPHP}-phpi.php file itself" $boldyellow
        sed -i "s|PHPUSERNAME|$PHPIUSER|" "/usr/local/nginx/html/${NPHP}-phpi.php"
        sed -i "s|PHPPASSWORD|$PHPIPASS|" "/usr/local/nginx/html/${NPHP}-phpi.php"
        chown nginx:nginx "/usr/local/nginx/html/${NPHP}-phpi.php"
        echo "*************************************************"
        cecho "PHP Info Login File: /usr/local/nginx/html/${NPHP}-phpi.php" $boldgreen
        cecho "PHP Info Login: ${hname}/${NPHP}-phpi.php" $boldgreen
        cecho "PHP Info Login username: ${PHPIUSER}" $boldgreen
        cecho "PHP Info Login password: ${PHPIPASS}" $boldgreen
        echo "*************************************************"
        echo
    fi
}
