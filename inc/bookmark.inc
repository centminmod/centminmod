bookmark() {

    # echo "check HOME=$HOME"
    # echo "check PWD=$PWD"
    echo
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
# Fast Network Bandwidth Transfer VPS Hosting >1-12Gbps at Linode
# New signups get free \$100 60 day credit with validated payment added
# https://www.linode.com/lp/refer/?r=f6d38ee057f8c18a3f508651b26af0e4ca1fbd46
###############################################################
# High Performance VPS Hosting at Upcloud
# New signups get free \$25 credits
# https://upcloud.com/signup/?promo=DPY36J
###############################################################
EOF

if [[ "$INITIALINSTALL" != [yY] ]]; then
  yumupdatechecker
  ngxver_checker
  gitenv_askupdate
fi

}