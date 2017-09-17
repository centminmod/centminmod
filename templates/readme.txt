#################################################################################
# Nginx vhost templates are located at /usr/local/src/centminmod/templates.
#################################################################################
# Editing instructions
# * DO NOT touch any variables with $ in front i.e. ${vhostname} or $vhostname
# * Any additional rules you add which need variables need to be espcaped with 
#   backslash in front example is wordpress permalink needs to set set in these
#   templates as:
#   
#   try_files \$uri \$uri/ /index.php?q=\$uri&\$args;
#
#   which will show up in live vhost otherwise bash will parse it as a variable
#
#   try_files $uri $uri/ /index.php?q=$uri&$args;
#################################################################################

vhost-non-wp-http.txt               - centmin.sh menu option 2 HTTP Port 80 template
vhost-non-wp-https.txt              - centmin.sh menu option 2 HTTPS Port 443 template
vhost-non-wp-nv-http.txt            - centmin.sh menu option 2 HTTP Port 80 template
vhost-non-wp-nv-https-default.txt   - centmin.sh menu option 2 HTTPS Port 443 template + HTTP to HTTPS redirect
vhost-non-wp-nv-https.txt           - centmin.sh menu option 2 HTTP Port 443 template
vhost-wp-http.txt                   - centmin.sh menu option 2 HTTP Port 80 template
vhost-wp-https.txt                  - centmin.sh menu option 2 HTTPS Port 443 template