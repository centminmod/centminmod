yumcleanup() {

echo "*************************************************"
cecho "* Remove unneeded YUM packages" $boldgreen
echo "*************************************************"
echo "please wait till completion."

  if [[ "$INITIALINSTALL" = [yY] ]]; then
    yum -y remove httpd* php* samba* exim* bind*
  fi

echo "*************************************************"
cecho "* yum clean all & update" $boldgreen
echo "*************************************************"
echo "please wait till completion."

   yum${CACHESKIP} -q clean all
   yum${CACHESKIP} -y update glibc\*
   yum${CACHESKIP} -y update yum\* rpm\* python\*
   yum${CACHESKIP} -q clean all
   yum${CACHESKIP} -y update

}

yuminstall() {
echo "*************************************************"
cecho "* Installing Development Tools" $boldgreen
echo "*************************************************"
echo "please wait for yum download and installs to complete."
echo "yum installs in less verbose mode so final install status is only shown.."
echo "...."
echo "......"
echo "........."

if [[ "$CENTOS_SIX" = '6' || "$CENTOS_SEVEN" = '7' ]]; then

  # do not install yum fastmirror plugin if not enough detected system memory available
  # for yum fastmirror operation
  if [[ "$(awk '/MemTotal/ {print $2}' /proc/meminfo)" -ge '1018000' && "$CENTOS_SEVEN" = '7' ]]; then
    if [[ $(rpm -q yum-plugin-fastestmirror >/dev/null 2>&1; echo $?) != '0' ]]; then
      yum -y install yum-plugin-fastestmirror${DISABLEREPO_DNF}
      sar_call
      fastmirror_tweaks
    fi
    if [[ $(rpm -q yum-plugin-downloadonly >/dev/null 2>&1; echo $?) != '0' ]]; then
      yum -y install yum-plugin-downloadonly${DISABLEREPO_DNF}
      sar_call
    fi
    if [[ $(rpm -q yum-plugin-security >/dev/null 2>&1; echo $?) != '0' ]]; then
      yum -y install yum-plugin-security${DISABLEREPO_DNF}
      sar_call
    fi
  elif [[ "$(awk '/MemTotal/ {print $2}' /proc/meminfo)" -ge '263000' ]]; then
    if [[ $(rpm -q yum-plugin-fastestmirror >/dev/null 2>&1; echo $?) != '0' ]]; then
      yum -y install yum-plugin-fastestmirror${DISABLEREPO_DNF}
      sar_call
      fastmirror_tweaks
    fi
    if [[ $(rpm -q yum-plugin-downloadonly >/dev/null 2>&1; echo $?) != '0' ]]; then
      yum -y install yum-plugin-downloadonly${DISABLEREPO_DNF}
      sar_call
    fi
    if [[ $(rpm -q yum-plugin-security >/dev/null 2>&1; echo $?) != '0' ]]; then
      yum -y install yum-plugin-security${DISABLEREPO_DNF}
      sar_call
    fi
  fi

if [[ $(rpm -q yum-plugin-priorities >/dev/null 2>&1; echo $?) != '0' ]]; then
  yum -y -q install yum-plugin-priorities${DISABLEREPO_DNF}
  sar_call
fi

if [[ $(rpm -q epel-release >/dev/null 2>&1; echo $?) != '0' ]]; then
  yum -y -q install epel-release${DISABLEREPO_DNF}
  sar_call
fi

if [[ -f /etc/yum.repos.d/epel.repo && -z "$(grep 'varnish-libs' /etc/yum.repos.d/epel.repo)" ]]; then
  sed -i 's|varnish|varnish varnish-libs|' /etc/yum.repos.d/epel.repo
fi
sed -i 's/metadata_expire=1h/metadata_expire=4h/g' /etc/yum.conf
  # centos 6 unlike centos 7 doesn't install CR yum repo by default
  # https://wiki.centos.org/AdditionalResources/Repositories/CR
  if [[ "$CENTOS_SIX" = '6' ]]; then
    yum -y -q install centos-release-cr${DISABLEREPO_DNF}
    sar_call
    sed -i 's/enabled=1/enabled=0/g' /etc/yum.repos.d/CentOS-CR.repo
    # echo "priority=1" >> /etc/yum.repos.d/CentOS-CR.repo
  fi
yumprior
else

if [[ $(rpm -q yum-priorities >/dev/null 2>&1; echo $?) != '0' ]]; then
  yum -y -q install yum-priorities${DISABLEREPO_DNF}
  sar_call
fi

if [[ $(rpm -q yum-fastestmirror >/dev/null 2>&1; echo $?) != '0' ]]; then
  yum -y -q install yum-fastestmirror${DISABLEREPO_DNF}
  sar_call
fi

if [[ $(rpm -q yum-downloadonly >/dev/null 2>&1; echo $?) != '0' ]]; then
  yum -y -q install yum-downloadonly${DISABLEREPO_DNF}
  sar_call
fi

if [[ $(rpm -q epel-release >/dev/null 2>&1; echo $?) != '0' ]]; then
  yum -y -q install epel-release${DISABLEREPO_DNF}
  sar_call
fi

sed -i 's/metadata_expire=1h/metadata_expire=4h/g' /etc/yum.conf
yumprior
fi

if [[ "$CENTOS_SEVEN" = '7' ]]; then
  if [[ $(rpm -q nmap-ncat >/dev/null 2>&1; echo $?) != '0' ]]; then
    time $YUMDNFBIN -y install nmap-ncat${DISABLEREPO_DNF}
    sar_call
  fi
else
  if [[ $(rpm -q nc >/dev/null 2>&1; echo $?) != '0' ]]; then
    time $YUMDNFBIN -y install nc libgcj${DISABLEREPO_DNF}
    sar_call
  fi
fi

yumcentosfivedownload

### smarter yum install update routine ###

if [[ $(rpm -q ImageMagick6 >/dev/null 2>&1; echo $?) != '0' ]]; then
  if [ -f /tmp/curlinstaller-yum ]; then
    echo
    echo "skip unneeded yum package check/installs for curl installer routines"
    echo
    PACKAGE='cracklib'
  else
    PACKAGE='virt-what gawk deltarpm cracklib ca-certificates pyOpenSSL python-dateutil yum-utils bash mlocate subversion rsyslog dos2unix net-tools imake bind-utils libatomic_ops-devel time coreutils autoconf cronie crontabs cronie-anacron gcc gcc-c++ automake libtool make libXext-devel unzip patch sysstat openssh flex bison file libtool-ltdl-devel krb5-devel libXpm-devel nano gmp-devel aspell-devel numactl lsof pkgconfig gdbm-devel tk-devel bluez-libs-devel iptables* rrdtool diffutils which perl-Test-Simple perl-ExtUtils-Embed perl-ExtUtils-MakeMaker perl-Time-HiRes perl-libwww-perl perl-Crypt-SSLeay perl-Net-SSLeay cyrus-imapd cyrus-sasl-md5 cyrus-sasl-plain strace cmake git net-snmp-libs net-snmp-utils iotop'
  fi
else
  if [ -f /tmp/curlinstaller-yum ]; then
    echo
    echo "skip unneeded yum package check/installs for curl installer routines"
    echo
    PACKAGE='cracklib'
  else
    PACKAGE='virt-what gawk deltarpm ca-certificates yum-plugin-security yum-utils bash mlocate subversion rsyslog dos2unix net-tools imake bind-utils libatomic_ops-devel time coreutils autoconf cronie crontabs cronie-anacron gcc gcc-c++ automake libtool make libXext-devel unzip patch sysstat openssh flex bison file libtool-ltdl-devel krb5-devel libXpm-devel nano gmp-devel aspell-devel numactl lsof pkgconfig gdbm-devel tk-devel bluez-libs-devel iptables* rrdtool diffutils which ImageMagick6 ImageMagick6-devel ImageMagick6-c++ ImageMagick6-c++-devel perl-Test-Simple perl-ExtUtils-MakeMaker perl-Time-HiRes perl-libwww-perl perl-Crypt-SSLeay perl-Net-SSLeay cyrus-imapd cyrus-sasl-md5 cyrus-sasl-plain strace cmake git net-snmp-libs net-snmp-utils iotop'
  fi
fi

declare -a array
array=(${PACKAGE})

ystarttime=$(TZ=UTC date +%s.%N)
{
# if curl one liner is used and detected should skip this yum install routine
# since it's already done by curl installer script
if [ ! -f /usr/local/src/curlinstall_yum.txt ]; then

for pkg in ${array[@]}
do
echo Processing package: $pkg
YUMINSTALLRPMCHECK=$(rpm -ql $pkg >/dev/null 2>&1; echo $?)
#echo $YUMINSTALLRPMCHECK
        if [[ "$YUMINSTALLRPMCHECK" = '0' ]]; then
echo "----------------------------------------------------------------------------------"
        # ${YUMDNFBIN}${CACHESKIP}-q -y update $pkg
        # ${YUMDNFBIN}${CACHESKIP} -q list $pkg
        echo "$pkg already installed"
echo "----------------------------------------------------------------------------------"
        else
echo "----------------------------------------------------------------------------------"
cecho "Installing $pkg" $boldyellow
echo "----------------------------------------------------------------------------------"
        ${YUMDNFBIN}${CACHESKIP} -q -y install $pkg${DISABLEREPO_DNF}
        # ${YUMDNFBIN}${CACHESKIP} -q list $pkg
echo "----------------------------------------------------------------------------------"
        fi
done
sar_call

fi # /usr/local/src/curlinstall_yum.txt

## grouped devels for faster install
if [[ $(rpm -q ImageMagick6 >/dev/null 2>&1; echo $?) != '0' ]]; then
  if [ -f /tmp/curlinstaller-yum ]; then
    echo
    echo "skip unneeded yum package check/installs for curl installer routines"
    echo
    PACKAGEG='{libvpx,libvpx-devel} {ImageMagick,ImageMagick-devel} {ImageMagick-c++,ImageMagick-c++-devel}'
  else
    PACKAGEG='{libvpx,libvpx-devel} {t1lib,t1lib-devel} {expect,expect-devel} {readline,readline-devel} {libedit,libedit-devel} {openssl,openssl-devel} {curl,curl-devel} {openldap,openldap-devel} {zlib,zlib-devel} {gd,gd-devel} {pcre,pcre-devel} {gettext,gettext-devel} {libidn,libidn-devel} {libjpeg,libjpeg-devel} {libpng,libpng-devel} {freetype,freetype-devel} {libxml2,libxml2-devel} {libxslt,libxslt-devel} {glib2,glib2-devel} {bzip2,bzip2-devel} {ncurses,ncurses-devel} {e2fsprogs,e2fsprogs-devel} {libc-client,libc-client-devel} {ImageMagick,ImageMagick-devel} {ImageMagick-c++,ImageMagick-c++-devel} {cyrus-sasl,cyrus-sasl-devel} {pam,pam-devel} {libaio,libaio-devel} {libevent,libevent-devel} {recode,recode-devel} {libtidy,libtidy-devel} {net-snmp,net-snmp-devel} {enchant,enchant-devel} {lua,lua-devel}'
  fi
else
  if [ -f /tmp/curlinstaller-yum ]; then
    echo
    echo "skip unneeded yum package check/installs for curl installer routines"
    echo
    PACKAGEG='{libvpx,libvpx-devel} {ImageMagick6,ImageMagick6-devel} {ImageMagick6-c++,ImageMagick6-c++-devel}'
  else
  PACKAGEG='{libvpx,libvpx-devel} {t1lib,t1lib-devel} {expect,expect-devel} {readline,readline-devel} {libedit,libedit-devel} {openssl,openssl-devel} {curl,curl-devel} {openldap,openldap-devel} {zlib,zlib-devel} {gd,gd-devel} {pcre,pcre-devel} {gettext,gettext-devel} {libidn,libidn-devel} {libjpeg,libjpeg-devel} {libpng,libpng-devel} {freetype,freetype-devel} {libxml2,libxml2-devel}  {libxslt,libxslt-devel} {glib2,glib2-devel} {bzip2,bzip2-devel} {ncurses,ncurses-devel} {e2fsprogs,e2fsprogs-devel} {libc-client,libc-client-devel} {ImageMagick6,ImageMagick6-devel} {ImageMagick6-c++,ImageMagick6-c++-devel} {cyrus-sasl,cyrus-sasl-devel} {pam,pam-devel} {libaio,libaio-devel} {libevent,libevent-devel} {recode,recode-devel} {libtidy,libtidy-devel} {net-snmp,net-snmp-devel} {enchant,enchant-devel} {lua,lua-devel}'
  fi
fi

declare -a arrayb
arrayb=(${PACKAGEG})

# if curl one liner is used and detected should skip this yum install routine
# since it's already done by curl installer script
if [ ! -f /usr/local/src/curlinstall_yum.txt ]; then

for pkgg in ${arrayb[@]}
do
echo Processing package: $pkgg
pkggp=$(echo $pkgg | sed -e 's|{||' -e 's|}||' -e 's|,| |')
YUMINSTALLRPMCHECK=$(rpm -qi $pkggp | awk '/Install Date/' | wc -l)
#echo $YUMINSTALLRPMCHECK
        if [[ "$YUMINSTALLRPMCHECK" = '2' ]]; then
echo "----------------------------------------------------------------------------------"
        # ${YUMDNFBIN}${CACHESKIP}-q -y update $pkgg
        # ${YUMDNFBIN}${CACHESKIP} -q list $pkgg
        echo "$pkgg already installed"
echo "----------------------------------------------------------------------------------"
        else
echo "----------------------------------------------------------------------------------"
cecho "Installing $pkgg" $boldyellow
echo "----------------------------------------------------------------------------------"
        ${YUMDNFBIN}${CACHESKIP} -q -y install $pkggp${DISABLEREPO_DNF}
        # ${YUMDNFBIN}${CACHESKIP} -q list $pkggp
echo "----------------------------------------------------------------------------------"
        fi
done
sar_call
fi # /usr/local/src/curlinstall_yum.txt


${YUMDNFBIN}${CACHESKIP} -y -q install mytop iftop${DISABLEREPO_DNF}
if [ -f /usr/bin/mytop ]; then
sed -i 's/"long|!"/"long|long_nums|l!"/g' /usr/bin/mytop
fi

if [ ! -f /proc/user_beancounters ]; then
  if [[ ! -f /usr/sbin/ipset || ! -f /usr/include/libipset/data.h ]]; then
    yum -y install ipset ipset-devel${DISABLEREPO_DNF}
    sar_call
  fi
fi

yum clean all -q
yumcentossixdownload
yumcentossevendownload
axelsetup
geoipinstall
customrpms

# use Remi's ImageMagicK 6.9.* package
if [[ "$REMIREPO_DISABLE" = [nN] &&  "$(uname -m)" != 'aarch64' ]]; then
  if [ -f /etc/yum.repos.d/remi.repo ]; then
    if [[ $(rpm -q ImageMagick >/dev/null 2>&1; echo $?) = '0' ]]; then
      echo
      echo "----------------------------------------------------------------------------------"
      cecho "Using Remi YUM repo ImageMagicK version" $boldyellow
      echo "----------------------------------------------------------------------------------"
      ${YUMDNFBIN} -y install lcms2-devel libwebp libwebp-devel OpenEXR-devel ilmbase-devel libGLU-devel libGL-devel mesa-libGL mesa-libGL-devel libXxf86vm libXxf86vm-devel --enablerepo=remi${DISABLEREPO_DNF}
      sar_call
  
      ${YUMDNFBIN} -y remove ImageMagick${DISABLEREPO_DNF}
      sar_call
  
      if [[ "$CENTOS_SIX" = '6' ]]; then
        # ${YUMDNFBIN} -y install libwebp libwebp-devel --skip-broken
        ${YUMDNFBIN} -y install ImageMagick6 ImageMagick6-devel ImageMagick6-c++ ImageMagick6-c++-devel --enablerepo=remi --disableplugin=priorities --skip-broken${DISABLEREPO_DNF}
        sar_call
      else
        ${YUMDNFBIN} -y install ImageMagick6 ImageMagick6-devel ImageMagick6-c++ ImageMagick6-c++-devel --enablerepo=remi --disableplugin=priorities${DISABLEREPO_DNF}
        sar_call
      fi
      
      echo
    fi

    if [[ "$(rpm -qa ImageMagick6)" ]]; then
      ${YUMDNFBIN} -y update ImageMagick6 ImageMagick6-devel ImageMagick6-c++ ImageMagick6-c++-devel --enablerepo=remi --disableplugin=priorities${DISABLEREPO_DNF}
      sar_call
    fi
  fi
fi

# remove /tmp/curlinstaller-yum
rm -rf /tmp/curlinstaller-yum

# duplicate
# yum -y update${DISABLEREPO_DNF}

} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_yumtimes_${DT}.log

yendtime=$(TZ=UTC date +%s.%N)
YINSTALLTIME=$(echo "scale=2;$yendtime - $ystarttime"|bc )
echo "" >> ${CENTMINLOGDIR}/centminmod_yumtimes_${DT}.log
echo "Total YUM Time: $YINSTALLTIME seconds" >> ${CENTMINLOGDIR}/centminmod_yumtimes_${DT}.log
echo "Total YUM Time: $YINSTALLTIME seconds" > ${CENTMINLOGDIR}/centminmod_profiletimes_${DT}.log
\cp /var/cache/yum/timedhosts.txt ${CENTMINLOGDIR}/timedhosts_${DT}.txt
ls -lah ${CENTMINLOGDIR}/centminmod_yumtimes_${DT}.log

}