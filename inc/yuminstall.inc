yumcleanup() {

echo "*************************************************"
cecho "* Remove unneeded YUM packages" $boldgreen
echo "*************************************************"
echo "please wait till completion."

  yum -y remove httpd* php* samba* exim* bind* 

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

if [[ "$CENTOSVER" = '6.0' || "$CENTOSVER" = '6.1' || "$CENTOSVER" = '6.2' || "$CENTOSVER" = '6.3' || "$CENTOSVER" = '6.4' || "$CENTOSVER" = '6.5' || "$CENTOSVER" = '6.6' || "$CENTOSVER" = '7.0' || "$CENTOSVER" = '7.1' || "$CENTOSVER" = '7.2' || "$CENTOSVER" = '7.3' || "$CENTOSVER" = '7.4' || "$CENTOSVER" = '7.5' || "$CENTOSVER" = '7.6' || "$CENTOSVER" = '7.7' ]]; then
yum -y -q install yum-plugin-priorities yum-plugin-fastestmirror yum-plugin-downloadonly epel-release
sed -i 's/metadata_expire=1h/metadata_expire=4h/g' /etc/yum.conf
yumprior
else
yum -y -q install yum-priorities yum-fastestmirror yum-downloadonly epel-release
sed -i 's/metadata_expire=1h/metadata_expire=4h/g' /etc/yum.conf
yumprior
fi

yumcentosfivedownload

### smarter yum install update routine ###

#yum -y -q update kernel-devel

if [[ -z "$(rpm -qa ImageMagick-last)" ]]; then

PACKAGE='deltarpm cracklib ca-certificates yum-plugin-security yum-utils bash mlocate subversion rsyslog dos2unix net-tools imake bind-utils libatomic_ops-devel time yum-downloadonly coreutils autoconf cronie crontabs cronie-anacron nc gcc gcc-c++ automake distcache libtool make libXext-devel unzip patch sysstat openssh flex bison file libgcj libtool-libs libtool-ltdl-devel krb5-devel libXpm-devel nano gmp-devel pspell-devel aspell-devel numactl lsof pkgconfig gdbm-devel tk-devel bluez-libs-devel iptables* rrdtool diffutils which perl-Test-Simple perl-ExtUtils-MakeMaker perl-Time-HiRes perl-libwww-perl perl-Crypt-SSLeay perl-Net-SSLeay perl-LWP-Protocol-https cyrus-imapd cyrus-sasl-md5 cyrus-sasl-plain strace cmake git net-snmp-libs net-snmp-utils iotop'

else

PACKAGE='deltarpm ca-certificates yum-plugin-security yum-utils bash mlocate subversion rsyslog dos2unix net-tools imake bind-utils libatomic_ops-devel time yum-downloadonly coreutils autoconf cronie crontabs cronie-anacron nc gcc gcc-c++ automake distcache libtool make libXext-devel unzip patch sysstat openssh flex bison file libgcj libtool-libs libtool-ltdl-devel krb5-devel libXpm-devel nano gmp-devel pspell-devel aspell-devel numactl lsof pkgconfig gdbm-devel tk-devel bluez-libs-devel iptables* rrdtool diffutils which ImageMagick-last ImageMagick-last-devel ImageMagick-last-c++ ImageMagick-last-c++-devel perl-Test-Simple perl-ExtUtils-MakeMaker perl-Time-HiRes perl-libwww-perl perl-Crypt-SSLeay perl-Net-SSLeay perl-LWP-Protocol-https cyrus-imapd cyrus-sasl-md5 cyrus-sasl-plain strace cmake git net-snmp-libs net-snmp-utils iotop'

fi

declare -a array
array=(${PACKAGE})

ystarttime=$(date +%s.%N)
{
for pkg in ${array[@]}
do
echo Processing package: $pkg
YUMINSTALLRPMCHECK=$(rpm -qa $pkg)
#echo $YUMINSTALLRPMCHECK
        if [ "$YUMINSTALLRPMCHECK" ]; then
echo "----------------------------------------------------------------------------------"
        # yum${CACHESKIP}-q -y update $pkg
        # yum${CACHESKIP} -q list $pkg
        echo "$pkg already installed"
echo "----------------------------------------------------------------------------------"
        else
echo "----------------------------------------------------------------------------------"
cecho "Installing $pkg" $boldyellow
echo "----------------------------------------------------------------------------------"
        yum${CACHESKIP} -q -y install $pkg
        # yum${CACHESKIP} -q list $pkg
echo "----------------------------------------------------------------------------------"
        fi
done

## grouped devels for faster install
if [[ -z "$(rpm -qa ImageMagick-last)" ]]; then

PACKAGEG='{libvpx,libvpx-devel} {t1lib,t1lib-devel} {expect,expect-devel} {readline,readline-devel} {libedit,libedit-devel} {openssl,openssl-devel} {curl,curl-devel} {openldap,openldap-devel} {zlib,zlib-devel} {gd,gd-devel} {pcre,pcre-devel} {gettext,gettext-devel} {libidn,libidn-devel} {libjpeg,libjpeg-devel} {libpng,libpng-devel} {freetype,freetype-devel} {libxml2,libxml2-devel} {glib2,glib2-devel} {bzip2,bzip2-devel} {ncurses,ncurses-devel} {e2fsprogs,e2fsprogs-devel} {libc-client,libc-client-devel} {ImageMagick,ImageMagick-devel} {ImageMagick-c++,ImageMagick-c++-devel} {cyrus-sasl,cyrus-sasl-devel} {pam,pam-devel} {libaio,libaio-devel} {libevent,libevent-devel} {recode,recode-devel} {libtidy,libtidy-devel} {net-snmp,net-snmp-devel} {enchant,enchant-devel} {lua,lua-devel}'

else

PACKAGEG='{libvpx,libvpx-devel} {t1lib,t1lib-devel} {expect,expect-devel} {readline,readline-devel} {libedit,libedit-devel} {openssl,openssl-devel} {curl,curl-devel} {openldap,openldap-devel} {zlib,zlib-devel} {gd,gd-devel} {pcre,pcre-devel} {gettext,gettext-devel} {libidn,libidn-devel} {libjpeg,libjpeg-devel} {libpng,libpng-devel} {freetype,freetype-devel} {libxml2,libxml2-devel} {glib2,glib2-devel} {bzip2,bzip2-devel} {ncurses,ncurses-devel} {e2fsprogs,e2fsprogs-devel} {libc-client,libc-client-devel} {ImageMagicklast,ImageMagick-last-devel} {ImageMagick-last-c++,ImageMagick-last-c++-devel} {cyrus-sasl,cyrus-sasl-devel} {pam,pam-devel} {libaio,libaio-devel} {libevent,libevent-devel} {recode,recode-devel} {libtidy,libtidy-devel} {net-snmp,net-snmp-devel} {enchant,enchant-devel} {lua,lua-devel}'

fi

declare -a arrayb
arrayb=(${PACKAGEG})

for pkgg in ${arrayb[@]}
do
echo Processing package: $pkgg
pkggp=$(echo $pkgg | sed -e 's|{||' -e 's|}||' -e 's|,| |')
YUMINSTALLRPMCHECK=$(rpm -qa $pkggp | wc -l)
#echo $YUMINSTALLRPMCHECK
        if [[ "$YUMINSTALLRPMCHECK" = '2' ]]; then
echo "----------------------------------------------------------------------------------"
        # yum${CACHESKIP}-q -y update $pkgg
        # yum${CACHESKIP} -q list $pkgg
        echo "$pkgg already installed"
echo "----------------------------------------------------------------------------------"
        else
echo "----------------------------------------------------------------------------------"
cecho "Installing $pkgg" $boldyellow
echo "----------------------------------------------------------------------------------"
        yum${CACHESKIP} -q -y install $pkggp
        # yum${CACHESKIP} -q list $pkggp
echo "----------------------------------------------------------------------------------"
        fi
done


yum${CACHESKIP} -y -q install mytop iftop
if [ -f /usr/bin/mytop ]; then
sed -i 's/"long|!"/"long|long_nums|l!"/g' /usr/bin/mytop
fi

if [ ! -f /proc/user_beancounters ]; then
  yum -y install ipset ipset-devel
fi

yum clean all -q
yumcentossixdownload
yumcentossevendownload
axelsetup
geoipinstall

# use Remi's ImageMagicK 6.9.* package
if [[ "$REMIREPO_DISABLE" = [nN] ]]; then
  if [ -f /etc/yum.repos.d/remi.repo ]; then
    if [[ "$(rpm -qa ImageMagick)" ]]; then
      echo
      echo "----------------------------------------------------------------------------------"
      cecho "Using Remi YUM repo ImageMagicK version" $boldyellow
      echo "----------------------------------------------------------------------------------"
      yum -y install lcms2-devel libwebp libwebp-devel OpenEXR-devel ilmbase-devel libGLU-devel libGL-devel mesa-libGL mesa-  libGL-devel libXxf86vm libXxf86vm-devel --enablerepo=remi
  
      yum -y remove ImageMagick
  
      if [[ "$CENTOSVER" = '6.0' || "$CENTOSVER" = '6.1' || "$CENTOSVER" = '6.2' || "$CENTOSVER" = '6.3' || "$CENTOSVER" = '6.4' || "$CENTOSVER" = '6.5' || "$CENTOSVER" = '6.6' || "$CENTOSVER" = '6.7' || "$CENTOSVER" = '6.8' || "$CENTOSVER" = '6.9' ]]; then
        # yum -y install libwebp libwebp-devel --skip-broken
        yum -y install ImageMagick-last ImageMagick-last-devel ImageMagick-last-c++ ImageMagick-last-c++-devel --enablerepo=remi --disableplugin=priorities --skip-broken
      else
        yum -y install ImageMagick-last ImageMagick-last-devel ImageMagick-last-c++ ImageMagick-last-c++-devel --enablerepo=remi --disableplugin=priorities
      fi
      
      echo
    fi

    if [[ "$(rpm -qa ImageMagick-last)" ]]; then
      yum -y update ImageMagick-last ImageMagick-last-devel ImageMagick-last-c++ ImageMagick-last-c++-devel --enablerepo=remi --disableplugin=priorities
    fi
  fi
fi

yum -y update

} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_yumtimes_${DT}.log

yendtime=$(date +%s.%N)
YINSTALLTIME=$(echo "scale=2;$yendtime - $ystarttime"|bc )
echo "" >> ${CENTMINLOGDIR}/centminmod_yumtimes_${DT}.log
echo "Total YUM Time: $YINSTALLTIME seconds" >> ${CENTMINLOGDIR}/centminmod_yumtimes_${DT}.log
echo "Total YUM Time: $YINSTALLTIME seconds" > ${CENTMINLOGDIR}/centminmod_profiletimes_${DT}.log
\cp /var/cache/yum/timedhosts.txt ${CENTMINLOGDIR}/timedhosts_${DT}.txt
ls -lah ${CENTMINLOGDIR}/centminmod_yumtimes_${DT}.log

}