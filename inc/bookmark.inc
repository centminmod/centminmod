bookmark() {

echo ""
  cecho "--------------------------------------------------------" $boldyellow
  cecho "Please bookmark:" $boldyellow
  cecho "--------------------------------------------------------" $boldyellow
cat <<EOF
###############################################################
* Getting Started Guide - https://centminmod.com/getstarted.html
* Find the latest Centmin Mod version - https://centminmod.com
* Centmin Mod FAQ - https://centminmod.com/faq.html
* Change Log - https://centminmod.com/changelog.html 
* Centmin Mod Blog https://blog.centminmod.com
* Community Forums https://community.centminmod.com (signup)
###############################################################
# Cheap VPS Hosting at Digitalocean 
# https://www.digitalocean.com/?refcode=c1cb367108e8
###############################################################
EOF

if [[ "$INITIALINSTALL" != [yY] ]]; then
  yumupdatechecker
  ngxver_checker
  gitenv_askupdate
fi

}