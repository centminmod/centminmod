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

if [[ "$CENTOSVER" = '6.0' || "$CENTOSVER" = '6.1' || "$CENTOSVER" = '6.2' || "$CENTOSVER" = '6.3' || "$CENTOSVER" = '6.4' || "$CENTOSVER" = '6.5' || "$CENTOSVER" = '6.6' || "$CENTOSVER" = '7.0' ]]; then
yum -y -q install yum-plugin-priorities yum-plugin-fastestmirror yum-plugin-downloadonly
sed -i 's/metadata_expire=1h/metadata_expire=4h/g' /etc/yum.conf
yumprior
else
yum -y -q install yum-priorities yum-fastestmirror yum-downloadonly
sed -i 's/metadata_expire=1h/metadata_expire=4h/g' /etc/yum.conf
yumprior
fi

yumcentosfivedownload

### smarter yum install update routine ###

#yum -y -q update kernel-devel

PACKAGE='ca-certificates yum-plugin-security yum-utils bash mlocate rsyslog dos2unix net-tools imake libvpx libvpx-devel t1lib t1lib-devel expect expect-devel bind-utils readline readline-devel libedit libedit-devel libatomic_ops-devel time yum-downloadonly coreutils autoconf cronie crontabs cronie-anacron nc gcc gcc-c++ automake distcache openssl openssl-devel curl curl-devel openldap openldap-devel libtool make libXext-devel unzip patch sysstat zlib zlib-devel libc-client-devel openssh gd gd-devel pcre pcre-devel flex bison file libgcj gettext gettext-devel e2fsprogs-devel libtool-libs libtool-ltdl-devel libidn libidn-devel krb5-devel libjpeg libjpeg-devel libpng libpng-devel freetype freetype-devel libxml2 libxml2-devel libXpm-devel glib2 glib2-devel bzip2 bzip2-devel nano ncurses ncurses-devel e2fsprogs gmp-devel pspell-devel aspell-devel numactl lsof pkgconfig gdbm-devel tk-devel bluez-libs-devel iptables* rrdtool diffutils libc-client libc-client-devel which ImageMagick ImageMagick-devel ImageMagick-c++ ImageMagick-c++-devel perl-Test-Simple perl-ExtUtils-MakeMaker perl-Time-HiRes perl-libwww-perl perl-Crypt-SSLeay perl-Net-SSLeay perl-LWP-Protocol-https cyrus-sasl cyrus-sasl-devel cyrus-imapd cyrus-sasl-md5 cyrus-sasl-plain strace pam pam-devel cmake libaio libaio-devel libevent libevent-devel git recode recode-devel libtidy libtidy-devel net-snmp net-snmp-devel net-snmp-libs net-snmp-utils enchant enchant-devel ipset ipset-devel'

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

yum${CACHESKIP} -y -q install mytop iftop iotop
if [ -f /usr/bin/mytop ]; then
sed -i 's/"long|!"/"long|long_nums|l!"/g' /usr/bin/mytop
fi

yum clean all -q
yumcentossixdownload
yumcentossevendownload
axelsetup

# use Remi's ImageMagicK 6.8.9 package
if [[ "$REMIREPO_DISABLE" = [nN] ]]; then
  if [ -f /etc/yum.repos.d/remi.repo ]; then
    echo
    echo "----------------------------------------------------------------------------------"
    cecho "Using Remi YUM repo ImageMagicK version" $boldyellow
    echo "----------------------------------------------------------------------------------"
    yum -y install lcms2-devel libwebp libwebp-devel OpenEXR-devel ilmbase-devel libGLU-devel libGL-devel mesa-libGL mesa-libGL-devel libXxf86vm libXxf86vm-devel --enablerepo=remi

    yum -y remove ImageMagick

    yum -y install ImageMagick-last ImageMagick-last-devel ImageMagick-last-c++ ImageMagick-last-c++-devel --enablerepo=remi --disableplugin=priorities
    echo
  fi
fi

yum -y update

} 2>&1 | tee ${CENTMINLOGDIR}/centminmod_yumtimes_${DT}.log

yendtime=$(date +%s.%N)
YINSTALLTIME=$(echo "scale=2;$yendtime - $ystarttime"|bc )
echo "" >> ${CENTMINLOGDIR}/centminmod_yumtimes_${DT}.log
echo "Total YUM Time: $YINSTALLTIME seconds" >> ${CENTMINLOGDIR}/centminmod_yumtimes_${DT}.log
\cp /var/cache/yum/timedhosts.txt ${CENTMINLOGDIR}/timedhosts_${DT}.txt
ls -lah ${CENTMINLOGDIR}/centminmod_yumtimes_${DT}.log

}