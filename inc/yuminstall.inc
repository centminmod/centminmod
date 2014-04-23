yuminstall() {
echo "*************************************************"
cecho "* Installing Development Tools" $boldgreen
echo "*************************************************"
echo "please wait for yum download and installs to complete."
echo "yum installs in less verbose mode so final install status is only shown.."
echo "...."
echo "......"
echo "........."

if [[ "$CENTOSVER" = '6.0' || "$CENTOSVER" = '6.1' || "$CENTOSVER" = '6.2' || "$CENTOSVER" = '6.3' || "$CENTOSVER" = '6.4' || "$CENTOSVER" = '6.5' ]]; then
yum -y -q install yum-plugin-priorities yum-plugin-fastestmirror yum-plugin-downloadonly
yumprior
else
yum -y -q install yum-priorities yum-fastestmirror yum-downloadonly
yumprior
fi

yumcentosfivedownload

### smarter yum install update routine ###

#yum -y -q update kernel-devel

PACKAGE='re2c expat expat-devel readline readline-devel libedit libedit-devel libatomic_ops-devel time yum-downloadonly coreutils autoconf autoconf213 cronie crontabs cronie-anacron nc gcc gcc-c++ automake distcache openssl openssl-devel curl curl-devel openldap openldap-devel libtool make libXext-devel unzip slocate patch sysstat gcc44 gcc44-c++ zlib zlib-devel libc-client-devel openssh gd gd-devel pcre pcre-devel flex bison file libgcj gettext gettext-devel e2fsprogs-devel libtool-libs libtool-ltdl-devel libidn libidn-devel krb5-devel libjpeg libjpeg-devel libpng libpng-devel freetype freetype-devel libxml2 libxml2-devel libXpm-devel libmcrypt libmcrypt-devel glib2 glib2-devel bzip2 bzip2-devel vim-minimal nano ncurses ncurses-devel e2fsprogs gmp-devel pspell-devel aspell-devel numactl lsof pkgconfig gdbm-devel tk-devel bluez-libs-devel iptables* rrdtool diffutils libc-client libc-client-devel which ImageMagick ImageMagick-devel ImageMagick-c++ ImageMagick-c++-devel perl-ExtUtils-MakeMaker perl-Time-HiRes cyrus-sasl cyrus-sasl-devel strace pam pam-devel cmake libaio libaio-devel libevent libevent-devel'

declare -a array
array=(${PACKAGE})

for pkg in ${array[@]}
do
echo Processing package: $pkg
YUMINSTALLRPMCHECK=`yum -q list installed $pkg | awk '/Installed/ {print $1}'`
#echo $YUMINSTALLRPMCHECK
        if [ "$YUMINSTALLRPMCHECK" == 'Installed' ]; then
echo "----------------------------------------------------------------------------------"
        yum -q -y update $pkg
        yum -q list $pkg
echo "----------------------------------------------------------------------------------"
        else
echo "----------------------------------------------------------------------------------"
cecho "Installing $pkg" $boldyellow
echo "----------------------------------------------------------------------------------"
        yum -q -y install $pkg
        yum -q list $pkg
echo "----------------------------------------------------------------------------------"
        fi
done

yum -y -q install mytop iftop iotop
if [ -f /usr/bin/mytop ]; then
sed -i 's/"long|!"/"long|long_nums|l!"/g' /usr/bin/mytop
fi

yumcentossixdownload

axelsetup

}