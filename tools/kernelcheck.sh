#!/bin/bash
#################################################################
# kernelcheck.sh for centminmod.com
# https://www.cloudlinux.com/all-products/product-overview/kernelcare?campaign=CentminMod
# written by George Liu
# whitelist ips
# http://kb.cloudlinux.com/2016/01/what-ips-to-whitelist-for-proper-kernelcare-work/
# csf -a 69.175.92.54 kernelcare
# csf -a 69.175.106.203 kernelcare
#################################################################
DT=$(date +"%d%m%y-%H%M%S")
KERNEL_VERBOSE='y'
KERNEL_DEBUG='n'
KERNEL_CHECKLOG='/tmp/kernel_check.log'
KERNELWGET_LINK='https://raw.githubusercontent.com/iseletsk/kernelchecker/master/py/kernelchecker.py'
KC_LINK='https://centminmod.com/kernelcare.html'
FORCE_IPVFOUR='y' # curl/wget commands through script force IPv4
#################################################################
# Setup Colours
black='\E[30;40m'
red='\E[31;40m'
green='\E[32;40m'
yellow='\E[33;40m'
blue='\E[34;40m'
magenta='\E[35;40m'
cyan='\E[36;40m'
white='\E[37;40m'

boldblack='\E[1;30;40m'
boldred='\E[1;31;40m'
boldgreen='\E[1;32;40m'
boldyellow='\E[1;33;40m'
boldblue='\E[1;34;40m'
boldmagenta='\E[1;35;40m'
boldcyan='\E[1;36;40m'
boldwhite='\E[1;37;40m'

Reset="tput sgr0"      #  Reset text attributes to normal
                       #+ without clearing screen.

cecho ()                     # Coloured-echo.
                             # Argument $1 = message
                             # Argument $2 = color
{
message=$1
color=$2
echo -e "$color$message" ; $Reset
return
}

# set locale temporarily to english
# due to some non-english locale issues
export LC_ALL=en_US.UTF-8
export LANG=en_US.UTF-8
export LANGUAGE=en_US.UTF-8
export LC_CTYPE=en_US.UTF-8

shopt -s expand_aliases
for g in "" e f; do
    alias ${g}grep="LC_ALL=C ${g}grep"  # speed-up grep, egrep, fgrep
done

if [ -f /etc/centminmod/custom_config.inc ]; then
  source /etc/centminmod/custom_config.inc
fi
if [[ "$FORCE_IPVFOUR" != [yY] ]]; then
  ipv_forceopt=""
else
  ipv_forceopt='4'
fi

kernelchecker_get() {
  mkdir -p /root/tools
  curl -${ipv_forceopt}Is --connect-timeout 5 --max-time 5 "$KERNELWGET_LINK" | grep 'HTTP\/' | grep '200' >/dev/null 2>&1
  WGET_CURLCHECK=$?
  if [[ "$WGET_CURLCHECK" = '0' ]]; then
    rm -rf /root/tools/kernelchecker.py
    wget -${ipv_forceopt}cnv -O /root/tools/kernelchecker.py "$KERNELWGET_LINK" >/dev/null 2>&1
  fi
}

#################################################################
kernelchecker_run() {
  if [ -f /root/tools/kernelchecker.py ]; then
    python /root/tools/kernelchecker.py | awk '{print $1 $2, $3}' | grep -v 'kernelcare:' > "$KERNEL_CHECKLOG"
    if [[ "$KERNEL_DEBUG" = [yY] ]]; then
      cat "$KERNEL_CHECKLOG"
    fi
    kc_latest=$(awk '/^latest:/ {print $2}' "$KERNEL_CHECKLOG")
    kc_current=$(awk '/^current:/ {print $2}' "$KERNEL_CHECKLOG")
    kc_distro=$(awk '/^distro:/ {print $2}' "$KERNEL_CHECKLOG")
    kc_needs_update=$(awk '/^needs_update:/ {print $2}' "$KERNEL_CHECKLOG")
    kc_latest_installed=$(awk '/^latest_installed:/ {print $2}' "$KERNEL_CHECKLOG")
    kc_latest_available=$(awk '/^latest_available:/ {print $2}' "$KERNEL_CHECKLOG")
    kc_inside_container=$(awk '/^inside_container:/ {print $2}' "$KERNEL_CHECKLOG")
    kc_kernelcare=$(awk '/^kernelcare:/ {print $2}' "$KERNEL_CHECKLOG")
    kc_installed=$(awk '/^installed:/ {print $2}' "$KERNEL_CHECKLOG")
    kc_up2date=$(awk '/^up2date:/ {print $2}' "$KERNEL_CHECKLOG")
    kc_supported=$(awk '/^supported:/ {print $2}' "$KERNEL_CHECKLOG")
    if [[ "$KERNEL_DEBUG" = [yY] ]]; then
      echo "DEBUG Mode Output:"
      echo "kc_latest = $kc_latest"
      echo "kc_current = $kc_current"
      echo "kc_distro = $kc_distro"
      echo "kc_needs_update = $kc_needs_update"
      echo "kc_latest_installed = $kc_latest_installed"
      echo "kc_latest_available = $kc_latest_available"
      echo "kc_inside_container = $kc_inside_container"
      echo "kc_installed = $kc_installed"
      echo "kc_up2date = $kc_up2date"
      echo "kc_supported = $kc_supported"
    fi
    kernelchecker_eval
    echo
  fi
}

#################################################################
kernelchecker_eval() {
# don't do anything if container based VPS i.e. OpenVZ
# when $kc_inside_container is True
if [[ "$kc_inside_container" = 'False' ]]; then
  if [[ "$kc_needs_update" = 'False' ]]; then
    # latest kernel is installed and properly rebooted afterwards
    if [[ "$KERNEL_VERBOSE" = [yY] ]]; then
      echo
      cecho "===============================================================================" $boldgreen
      echo " system kernel is up to date, nothing to do"
      cecho "===============================================================================" $boldgreen 
    fi
  elif [[ "$kc_up2date" = 'True' ]]; then
    if [[ "$KERNEL_VERBOSE" = [yY] ]]; then
      echo
      cecho "===============================================================================" $boldgreen
      echo " kernelcare kernel is up to date, nothing to do"
      cecho "===============================================================================" $boldgreen 
    fi
  elif [[ "$kc_needs_update" = 'True' ]]; then
    if [[ "$KERNEL_VERBOSE" = [yY] ]]; then
      echo
      cecho "===============================================================================" $boldgreen
      echo " newer kernel is available or recently updated"
      echo " a system reboot is needed"
      echo " please run commands below to check kernel yum package history (Begin time),"
      echo " yum update and then reboot server (if Begin time is recent):"
      echo
      echo "  yum history package-info kernel"
      echo "  yum update"
      cecho "===============================================================================" $boldgreen 
    fi
    if [[ "$kc_installed" = 'True' ]]; then
      if [[ "$KERNEL_VERBOSE" = [yY] ]]; then
        echo
        cecho "===============================================================================" $boldgreen 
        echo " please update your kernelcare kernel with command below:"
        echo
        echo "  kcarectl --update"
        cecho "===============================================================================" $boldgreen 
      fi
    elif [[ "$kc_supported" = 'True' ]]; then
      if [[ "$KERNEL_VERBOSE" = [yY] ]]; then
        echo
        cecho "===============================================================================" $boldgreen 
        echo " kernel updates tradiitionally require server reboots"
        echo " such reboots cause downtime for your visitors & sites"
        echo
        cecho "===============================================================================" $boldgreen 
        echo " Use KernelCare for automated rebootless kernel updates"
        echo " you can purchase & install KernelCare for rebootless"
        echo " kernel updates with the latest security kernel patches"
        echo " KernelCare automatically checks for kernel updates every"
        echo " 4hrs"
        echo " Centmin Mod 123.09beta01+ support KernelCare checks too!"
        echo " For more info go to $KC_LINK"
        cecho "===============================================================================" $boldgreen 
        echo
      fi
    elif [[ "$kc_latest_installed" = 'False' ]]; then
      if [[ "$KERNEL_VERBOSE" = [yY] ]]; then
        echo
        cecho "===============================================================================" $boldgreen 
        echo " system kernel is latest installed, but requires a system reboot"
      fi
    # else
    #   if [[ "$KERNEL_VERBOSE" = [yY] ]]; then
    #   echo
    #   cecho "===============================================================================" $boldgreen
    #   echo " newer kernel is available, system reboot needed"
    #   echo " please run command below then reboot server:"
    #   echo
    #   echo "  yum update"
    #   cecho "===============================================================================" $boldgreen 
    #  fi
    fi
  fi
fi
}

#################################################################
kernelchecker_get
kernelchecker_run
exit