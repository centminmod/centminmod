#!/bin/sh
SCRIPT_VERSION='1.0.3mod'
SCRIPT_DATE='1/09/2011'
SCRIPT_AUTHOR='BTCentral'
COPYRIGHT="Copyright 2011 BTCentral"
DISCLAIMER='This software is provided "as is" in the hope that it will be useful, but WITHOUT ANY WARRANTY, to the extent permitted by law; without even the implied warranty of MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.'
LIBUNWIND_VERSION='0.99'     # Use this version of libunwind
GPERFTOOLS_VERSION='1.8.2'     # Use this version of google-perftools
OPENSSL_VERSION='1.0.0d'     # Use this version of OpenSSL
PCRE_VERSION='8.12'          # Use this version of PCRE library

KEYPRESS_PARAM='-s -n1 -p'   # Read a keypress without hitting ENTER.
		# -s means do not echo input.
		# -n means accept only N characters of input.
		# -p means echo the following prompt before reading input

MACHINE_TYPE=`uname -m` # Used to detect if OS is 64bit or not.
ASKCMD="read $KEYPRESS_PARAM "
CUR_DIR=`pwd` # Get current directory.
###############################################################
# FUNCTIONS

ASK () {
keystroke=''
while [[ "$keystroke" != [yYnN] ]]
do
    $ASKCMD "$1" keystroke
    echo "$keystroke";
done

key=$(echo $keystroke)
}

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
# END FUNCTIONS
################################################################
# SCRIPT START
#
cecho "**********************************************************************" $boldyellow
cecho "* Nginx Update script - Included in Centmin Extras" $boldgreen
cecho "* Version: $SCRIPT_VERSION - Date: $SCRIPT_DATE - $COPYRIGHT" $boldgreen
cecho "**********************************************************************" $boldyellow
echo " "
cecho "This software comes with no warranty of any kind. You are free to use" $boldyellow
cecho "it for both personal and commercial use as licensed under the GPL." $boldyellow
echo " "
ASK "Would you like to continue? [y/n] "   
if [[ "$key" = [nN] ]];
then
    exit 0
fi

DIR_TMP="/svr-setup"
mkdir $DIR_TMP

echo -n "Install which version of Nginx? (Type version and press Enter): "
read ngver

    echo "*************************************************"
    cecho "* Updating nginx" $boldgreen
    echo "*************************************************"

    cd $DIR_TMP

    # nginx Modules / Prerequisites
	cecho "Installing nginx Modules / Prerequisites..." $boldgreen

    # Install libunwind
    echo "Compiling libunwind..."
    if [ -s libunwind-${LIBUNWIND_VERSION}.tar.gz ]; then
        cecho "libunwind ${LIBUNWIND_VERSION} Archive found, skipping download..." $boldgreen 
    else
        wget -c http://download.savannah.gnu.org/releases/libunwind/libunwind-${LIBUNWIND_VERSION}.tar.gz --tries=3
    fi

    tar xvzf libunwind-${LIBUNWIND_VERSION}.tar.gz
    cd libunwind-${LIBUNWIND_VERSION}
    ./configure
    make
    make install

    # Install google-perftools
    cd $DIR_TMP

    echo "Compiling google-perftools..."
    if [ -s google-perftools-${GPERFTOOLS_VERSION}.tar.gz ]; then
        cecho "google-perftools ${GPERFTOOLS_VERSION} Archive found, skipping download..." $boldgreen
    else
        wget -c http://google-perftools.googlecode.com/files/google-perftools-${GPERFTOOLS_VERSION}.tar.gz --tries=3
    fi

    tar xvzf google-perftools-${GPERFTOOLS_VERSION}.tar.gz
    cd google-perftools-${GPERFTOOLS_VERSION}
    ./configure --enable-frame-pointers
    make
    make install
    echo "/usr/local/lib" > /etc/ld.so.conf.d/usr_local_lib.conf
    /sbin/ldconfig

    # Install OpenSSL
    cd $DIR_TMP

    echo "Compiling OpenSSL..."
    if [ -s openssl-${OPENSSL_VERSION}.tar.gz ]; then
        cecho "openssl ${OPENSSL_VERSION} Archive found, skipping download..." $boldgreen
    else
        wget -c http://www.openssl.org/source/openssl-${OPENSSL_VERSION}.tar.gz --tries=3
    fi

    tar xvzf openssl-${OPENSSL_VERSION}.tar.gz
    cd openssl-${OPENSSL_VERSION}
    ./config --prefix=/usr/local --openssldir=/usr/local/ssl
    make
    make install

    ./config shared --prefix=/usr/local --openssldir=/usr/local/ssl
    make clean
    make 
    make install

    # Install PCRE
    cd $DIR_TMP

    echo "Compiling PCRE..."
    if [ -s pcre-${PCRE_VERSION}.tar.gz ]; then
        cecho "pcre ${PCRE_VERSION} Archive found, skipping download..." $boldgreen
    else
        wget -c ftp://ftp.csx.cam.ac.uk/pub/software/programming/pcre/pcre-${PCRE_VERSION}.tar.gz --tries=3
    fi

    tar xvzf pcre-${PCRE_VERSION}.tar.gz
    cd pcre-${PCRE_VERSION}
    ./configure
    make
    make install

    # nginx Modules
    cd $DIR_TMP

    if [ -s ngx-fancyindex-0.3.1.tar.gz ]; then
        cecho "ngx-fancyindex 0.3.1 Archive found, skipping download..." $boldgreen
    else
        wget -c http://gitorious.org/ngx-fancyindex/ngx-fancyindex/archive-tarball/v0.3.1 -O ngx-fancyindex-0.3.1.tar.gz --tries=3
    fi


    tar zvxf ngx-fancyindex-0.3.1.tar.gz

    if [ -s ngx_cache_purge-1.3.tar.gz ]; then
        cecho "ngx_cache_purge 1.3 Archive found, skipping download..." $boldgreen
    else
        wget -c http://labs.frickle.com/files/ngx_cache_purge-1.3.tar.gz
    fi

    tar zvxf ngx_cache_purge-1.3.tar.gz

    if [ -s Nginx-accesskey-2.0.3.tar.gz ]; then
        cecho "Nginx-accesskey 2.0.3 Archive found, skipping download..." $boldgreen
    else
        wget -c http://wiki.nginx.org/images/5/51/Nginx-accesskey-2.0.3.tar.gz
    fi

    tar zvxf Nginx-accesskey-2.0.3.tar.gz

    # Install nginx
    cd $DIR_TMP

    echo "Compiling nginx..."
    if [ -s nginx-${ngver}.tar.gz ]; then
        cecho "nginx ${ngver} Archive found, skipping download..." $boldgreen
    else
        wget -c "http://nginx.org/download/nginx-${ngver}.tar.gz"
    fi

    if [ ${MACHINE_TYPE} == 'x86_64' ];
    then
        MBIT='64'
    else
        MBIT='32'
    fi

    tar xvfz nginx-${ngver}.tar.gz
    cd nginx-${ngver}

    ASK "Would you like to compile nginx with IPv6 support? [y/n] "
    if [[ "$key" = [yY] ]]; then

./configure --sbin-path=/usr/local/sbin --conf-path=/usr/local/nginx/conf/nginx.conf --with-ipv6 --with-http_ssl_module --with-http_gzip_static_module --with-http_stub_status_module --with-http_sub_module --with-http_addition_module --with-http_secure_link_module --with-http_flv_module --with-http_realip_module --add-module=../ngx-fancyindex-ngx-fancyindex --add-module=../ngx_cache_purge-1.3 --add-module=../nginx-accesskey-2.0.3 --with-google_perftools_module --with-openssl=../openssl-1.0.0d

    else

./configure --sbin-path=/usr/local/sbin --conf-path=/usr/local/nginx/conf/nginx.conf --with-http_ssl_module --with-http_gzip_static_module --with-http_stub_status_module --with-http_sub_module --with-http_addition_module --with-http_secure_link_module --with-http_flv_module --with-http_realip_module --add-module=../ngx-fancyindex-ngx-fancyindex --add-module=../ngx_cache_purge-1.3 --add-module=../nginx-accesskey-2.0.3 --with-google_perftools_module --with-openssl=../openssl-1.0.0d
    fi    

    make
    /etc/init.d/nginx stop
    make install
    /etc/init.d/nginx start

    echo "*************************************************"
    cecho "* nginx updated" $boldgreen
    echo "*************************************************"

echo " "

rm -rf $DIR_TMP

exit 0