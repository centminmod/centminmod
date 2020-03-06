csfcron_setup() {
    if [[ -f /usr/local/src/centminmod/tools/csfcf.sh && -f /etc/csf/csf.conf && "$(csf -l >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
        # if CSF Firewall exists and running ensure Cloudflare IP updater cronjob
        # is setup and whitelisted in CSF Firewall for new Centmin Mod initial installs
        # https://community.centminmod.com/threads/6241/
        crontab -l > cronjoblist
        sed -i '/csfcf.sh/d' cronjoblist
        echo "22 */12 * * * /usr/local/src/centminmod/tools/csfcf.sh auto >/dev/null 2>&1" >> cronjoblist
        crontab cronjoblist
    fi
}

csf_maxmind_geolite_two_download() {
    csf_maxmind_silent=$1
    if [[ -f /etc/csf/csf.conf && -d /var/lib/csf/ ]] && [[ ! -f /var/lib/csf/Geo/GeoLite2-Country-Blocks-IPv4.csv || ! -f /var/lib/csf/Geo/GeoLite2-Country-Blocks-IPv6.csv || ! -f GeoLite2-Country-Locations-en.csv ]]; then
        if [[ "$csf_maxmind_silent" = 'silent' ]]; then
            wget -q -4 https://centminmod.com/centminmodparts/geoip2-lite/csf-geolite2.tar.gz -O /svr-setup/csf-geolite2.tar.gz
            tar xf /svr-setup/csf-geolite2.tar.gz -C /var/lib/csf/
            csf --profile backup cmm-before-maxmind-csf-database-download >/dev/null 2>&1
            sed -i 's|CC_INTERVAL = .*|CC_INTERVAL = "14"|' /etc/csf/csf.conf
            csf --profile backup cmm-after-maxmind-csf-database-download >/dev/null 2>&1
        else
            wget -4 https://centminmod.com/centminmodparts/geoip2-lite/csf-geolite2.tar.gz -O /svr-setup/csf-geolite2.tar.gz
            tar xvf /svr-setup/csf-geolite2.tar.gz -C /var/lib/csf/
            csf --profile backup cmm-before-maxmind-csf-database-download >/dev/null 2>&1
            sed -i 's|CC_INTERVAL = .*|CC_INTERVAL = "14"|' /etc/csf/csf.conf
            csf --profile backup cmm-after-maxmind-csf-database-download >/dev/null 2>&1
            csf --profile list
            grep '^CC_INTERVAL' /etc/csf/csf.conf
            # csf -ra
            tail -20 /var/log/lfd.log
            ls -lAh /var/lib/csf/Geo
        fi
        if [[ "$(csf -l >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
            csf -ra >/dev/null 2>&1
        fi
    fi
}

csf_maxmind_rego() {
    if [ -f /etc/csf/csf.conf ]; then
        # Use maxmind Country database instead of db-ip more accurate identification of
        # IP's geolocation
        #
        if [[ "$MM_CSF_SRC" = [yY] ]]; then
            if [[ ! "$(grep 'CC_SRC = "1"' /etc/csf/csf.conf)" ]]; then
                csf --profile backup before_cc_src_1_switch
                sed -i 's|CC_SRC = .*|CC_SRC = "1"|' /etc/csf/csf.conf
                csf --profile backup after_cc_src_1_switch
            fi
        elif [[ "$MM_CSF_SRC" = [nN] ]]; then
            if [[ ! "$(grep 'CC_SRC = "2"' /etc/csf/csf.conf)" ]]; then
                csf --profile backup before_cc_src_2_switch
                sed -i 's|CC_SRC = .*|CC_SRC = "2"|' /etc/csf/csf.conf
                csf --profile backup after_cc_src_2_switch
            fi
        fi
        # https://blog.maxmind.com/2019/12/18/significant-changes-to-accessing-and-using-geolite2-databases/
        # https://dev.maxmind.com/geoip/geoipupdate/#Direct_Downloads
        # check if MM_LICENSE_KEY variable is empty in /etc/csf/csf.conf
        if [ ! -f /etc/centminmod/custom_config.inc ]; then
            CHECK_CMM_MM_LICENSE_KEY=''
        else
            CHECK_CMM_MM_LICENSE_KEY=$(awk -F '=' '/MM_LICENSE_KEY/ {print $2}' /etc/centminmod/custom_config.inc | sed -e 's| ||g' | sed -e 's|"||g' -e "s|'||g")
        fi
        CHECK_CSF_MM_LICENSE_KEY=$(awk -F '=' '/MM_LICENSE_KEY/ {print $2}' /etc/csf/csf.conf | sed -e 's| ||g' | sed -e 's|"||g' -e "s|'||g")
        CHECK_CSF_MM_LICENSE_KEY_VARCHECK=$(awk -F '=' '/MM_LICENSE_KEY/ {print $1}' /etc/csf/csf.conf | sed -e 's| ||g' | sed -e 's|"||g' -e "s|'||g")
        if [[ "$MM_LICENSE_KEY" && "$CHECK_CMM_MM_LICENSE_KEY" ]] && [ ! "$CHECK_CSF_MM_LICENSE_KEY" ]; then
            # if MM_LICENSE_KEY variable is set in Centmin Mod's persistent config file
            # at /etc/centminmod/custom_config.inc via CHECK_CMM_MM_LICENSE_KEY
            # variable check and not set in CSF config file
            # at /etc/csf/csf.conf, then ensure CSF Firewall's MM_LICENSE_KEY
            # variable is populated as well using CHECK_CMM_MM_LICENSE_KEY
            csf --profile backup cmm-before-maxmind-rego >/dev/null 2>&1
            sed -i "s|MM_LICENSE_KEY = .*|MM_LICENSE_KEY = \"${MM_LICENSE_KEY}\"|" /etc/csf/csf.conf
            csf --profile backup cmm-after-maxmind-rego >/dev/null 2>&1

            # if MM_LICENSE_KEY is set, we can change CC_INTERVAL back to default 14 days for
            # lfd geolite2 database download updates
            csf --profile backup cmm-before-maxmind-csf-cc-interval-reset >/dev/null 2>&1
            sed -i 's|CC_INTERVAL = .*|CC_INTERVAL = "14"|' /etc/csf/csf.conf
            csf --profile backup cmm-after-maxmind-csf-cc-interval-reset >/dev/null 2>&1
            if [[ "$(csf -l >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
                csf -ra >/dev/null 2>&1
            fi
        elif [[ "$MM_LICENSE_KEY" && "$CHECK_CMM_MM_LICENSE_KEY" ]] && [[ "$CHECK_CSF_MM_LICENSE_KEY" = 'k0sP8JPgZm6i0sOF' ]]; then
            # if MM_LICENSE_KEY variable is set in Centmin Mod's persistent config file
            # at /etc/centminmod/custom_config.inc via CHECK_CMM_MM_LICENSE_KEY
            # variable check and shared Maxmind API key is already set in CSF config file
            # at /etc/csf/csf.conf, then ensure CSF Firewall's MM_LICENSE_KEY
            # variable is populated with /etc/centminmod/custom_config.inc version
            csf --profile backup cmm-before-maxmind-rego-override >/dev/null 2>&1
            sed -i "s|MM_LICENSE_KEY = .*|MM_LICENSE_KEY = \"${MM_LICENSE_KEY}\"|" /etc/csf/csf.conf
            csf --profile backup cmm-after-maxmind-rego-override >/dev/null 2>&1

            # if MM_LICENSE_KEY is set, we can change CC_INTERVAL back to default 14 days for
            # lfd geolite2 database download updates
            csf --profile backup cmm-before-maxmind-csf-cc-interval-reset-override >/dev/null 2>&1
            sed -i 's|CC_INTERVAL = .*|CC_INTERVAL = "14"|' /etc/csf/csf.conf
            csf --profile backup cmm-after-maxmind-csf-cc-interval-reset-override >/dev/null 2>&1
            if [[ "$(csf -l >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
                csf -ra >/dev/null 2>&1
            fi
        elif [[ "$MM_LICENSE_KEY" && ! "$CHECK_CMM_MM_LICENSE_KEY" ]] && [ ! "$CHECK_CSF_MM_LICENSE_KEY" ]; then
            # if MM_LICENSE_KEY variable is set in centmin.sh for shared Maxmind API key
            csf --profile backup cmm-before-maxmind-rego-sharedkey >/dev/null 2>&1
            sed -i "s|MM_LICENSE_KEY = .*|MM_LICENSE_KEY = \"${MM_LICENSE_KEY}\"|" /etc/csf/csf.conf
            csf --profile backup cmm-after-maxmind-rego-sharedkey >/dev/null 2>&1

            # if MM_LICENSE_KEY is set, we can change CC_INTERVAL back to default 14 days for
            # lfd geolite2 database download updates
            csf --profile backup cmm-before-maxmind-csf-cc-interval-reset-sharedkey >/dev/null 2>&1
            sed -i 's|CC_INTERVAL = .*|CC_INTERVAL = "14"|' /etc/csf/csf.conf
            csf --profile backup cmm-after-maxmind-csf-cc-interval-reset-sharedkey >/dev/null 2>&1
            if [[ "$(csf -l >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
                csf -ra >/dev/null 2>&1
            fi
        elif [[ ! "$CHECK_CMM_MM_LICENSE_KEY" && ! "$CHECK_CSF_MM_LICENSE_KEY" ]]; then
            # if MM_LICENSE_KEY isn't set in /etc/centminmod/custom_config.inc & /etc/csf/csf.conf
            # download last publicly available geolite2 databases for CSF Firewall
            # as at Dec 23, 2019 & extend CC_INTERVAL to 365 days before next geolite2 database update to give
            # time to develop alternative arrangements
            csf_maxmind_geolite_two_download silent
        fi
    fi
}

csfinstalls() {

    # centos 6.6 minimal bug
    # https://bugzilla.redhat.com/show_bug.cgi?id=1161682
    # if [[ "$CENTOS_SIX" = '6' && ! -f /etc/sysconfig/iptables ]]; then
    #     yum -y install system-config-firewall-base
    #     lokkit --default=server
    #     service iptables restart
    # fi

# CSF is compulsory
CSFINSTALLOK='y'

#ASK "Install CSF firewall script ? [y/n] "
if [[ "$CSFINSTALLOK" = [yY] ]]; then

    echo "*************************************************"
    cecho "* Installing CSF firewall... " $boldgreen
    echo "*************************************************"
    echo "Installing..."

    cd $DIR_TMP

#download csf tarball

    if [[ $(rpm -q perl-Crypt-SSLeay >/dev/null 2>&1; echo $?) != '0' ]] || [[ $(rpm -q perl-Net-SSLeay >/dev/null 2>&1; echo $?) != '0' ]]; then
        yum${CACHESKIP} -y install perl-libwww-perl perl-Crypt-SSLeay perl-Net-SSLeay
        sar_call
    elif [[ -z "$(rpm -qa perl-libwww-perl)" ]]; then
        yum${CACHESKIP} -y install perl-libwww-perl
        sar_call
    fi
    if [[ "$CENTOS_SEVEN" = '7' ]]; then
        if [[ $(rpm -q perl-LWP-Protocol-https >/dev/null 2>&1; echo $?) != '0' ]]; then
            yum${CACHESKIP} -y install perl-LWP-Protocol-https
            sar_call
        fi
    fi

    #tar xzf csf.tgz
    cd "$DIR_TMP/csf"
    sh install.sh
    sar_call

    # echo "Test IP Tables Modules..."

    # perl /etc/csf/csftest.pl
    cp -a /etc/csf/csf.conf /etc/csf/csf.conf-bak
    csf --profile backup initial-default
    csf --profile list

    echo "CSF adding memcached, varnish ports to csf.allow list..."
    sed -i 's/20,21,22,25,53,80,110,143,443,465,587,993,995/20,21,22,25,53,80,110,111,143,161,443,465,587,993,995,1110,1186,1194,2049,81,9418,30001:50011/g' /etc/csf/csf.conf

sed -i "s/TCP_OUT = \"/TCP_OUT = \"8080,2525,465,111,2049,1110,1194,9418,/g" /etc/csf/csf.conf
sed -i "s/TCP6_OUT = \"/TCP6_OUT = \"8080,2525,465,/g" /etc/csf/csf.conf
sed -i "s/UDP_IN = \"/UDP_IN = \"67,68,111,2049,1110,33434:33534,/g" /etc/csf/csf.conf
sed -i "s/UDP_OUT = \"/UDP_OUT = \"67,68,111,2049,1110,33434:33534,/g" /etc/csf/csf.conf
sed -i "s/DROP_NOLOG = \"67,68,/DROP_NOLOG = \"/g" /etc/csf/csf.conf

    egrep '^UDP_|^TCP_|^DROP_NOLOG' /etc/csf/csf.conf
    
    echo "Disabling CSF Testing mode (activates firewall)..."
    sed -i 's/TESTING = "1"/TESTING = "0"/g' /etc/csf/csf.conf

csf_maxmind_rego
# csf_maxmind_geolite_two_download
csftweaks

#######################################################
# check to see if csf.pignore already has custom apps added

CSFPIGNORECHECK=`grep -E '(user:nginx|user:nsd|exe:/usr/local/bin/memcached)' /etc/csf/csf.pignore`

if [[ -z $CSFPIGNORECHECK ]]; then

    echo "Adding Applications/Users to CSF ignore list..."
cat >>/etc/csf/csf.pignore<<EOF
pexe:/usr/local/lsws/bin/lshttpd.*
pexe:/usr/local/lsws/fcgi-bin/lsphp.*
exe:/usr/local/bin/memcached
cmd:/usr/local/bin/memcached
exe:/usr/libexec/openssh/sftp-server
cmd:/usr/libexec/openssh/sftp-server
user:mysql
exe:/usr/sbin/mysqld 
cmd:/usr/sbin/mysqld
user:varnish
exe:/usr/sbin/varnishd
cmd:/usr/sbin/varnishd
exe:/sbin/portmap
cmd:portmap
exe:/usr/libexec/gdmgreeter
cmd:/usr/libexec/gdmgreeter
exe:/usr/sbin/avahi-daemon
cmd:avahi-daemon
exe:/sbin/rpc.statd
cmd:rpc.statd
exe:/usr/libexec/hald-addon-acpi
cmd:hald-addon-acpi
user:nsd
user:nginx
user:ntp
user:dbus
user:smmsp
user:postfix
user:dovecot
user:www-data
user:spamfilter
exe:/sbin/rpcbind
exe:/usr/sbin/rpcbind
exe:/usr/libexec/dovecot/imap
exe:/usr/libexec/dovecot/pop3
exe:/usr/libexec/dovecot/anvil
exe:/usr/libexec/dovecot/auth
exe:/usr/libexec/dovecot/pop3-login
exe:/usr/libexec/dovecot/imap-login
exe:/usr/libexec/postfix
exe:/usr/libexec/postfix/bounce
exe:/usr/libexec/postfix/discard
exe:/usr/libexec/postfix/error
exe:/usr/libexec/postfix/flush
exe:/usr/libexec/postfix/local
exe:/usr/libexec/postfix/smtp
exe:/usr/libexec/postfix/smtpd
exe:/usr/libexec/postfix/pickup
exe:/usr/libexec/postfix/tlsmgr
exe:/usr/libexec/postfix/qmgr
exe:/usr/libexec/postfix/virtual
exe:/usr/libexec/postfix/proxymap
exe:/usr/libexec/postfix/anvil
exe:/usr/libexec/postfix/lmtp
exe:/usr/libexec/postfix/scache
exe:/usr/libexec/postfix/cleanup
exe:/usr/libexec/postfix/trivial-rewrite
exe:/usr/libexec/postfix/master
exe:/home/virtfs/reviews/opt/cpanel/ea-php56/root/usr/bin/php-cgi
cmd:/opt/cpanel/ea-php56/root/usr/bin/php-cgi
exe:/home/virtfs/reviews/opt/cpanel/ea-php55/root/usr/bin/php-cgi
cmd:/opt/cpanel/ea-php55/root/usr/bin/php-cgi
exe:/home/virtfs/reviews/opt/cpanel/ea-php70/root/usr/bin/php-cgi
cmd:/opt/cpanel/ea-php70/root/usr/bin/php-cgi
exe:/home/virtfs/reviews/opt/cpanel/ea-php71/root/usr/bin/php-cgi
cmd:/opt/cpanel/ea-php71/root/usr/bin/php-cgi
exe:/home/virtfs/reviews/opt/cpanel/ea-php72/root/usr/bin/php-cgi
cmd:/opt/cpanel/ea-php72/root/usr/bin/php-cgi
EOF

fi # check to see if csf.pignore already has custom apps added

    csf -u
    cmchkconfig csf on
    cmservice csf restart
    csf -ra

    cmchkconfig lfd on
    cmservice lfd start

# if CentOS 7 is detected disable firewalld in favour 
# of csf iptables ip6tables for now
if [[ "$CENTOS_EIGHT" = '8' || "$CENTOS_SEVEN" = '7' ]]; then
    if [[ "$FIREWALLD_DISABLE" = [yY] ]]; then
        # disable firewalld
        if [[ "$CENTOS_EIGHT" = '8' ]]; then
            systemctl disable firewalld nftables
            systemctl stop firewalld nftables
        else
            systemctl disable firewalld
            systemctl stop firewalld
        fi
    
        # install iptables-services package
        if [ ! -f /usr/lib/systemd/system/iptables.service ]; then
            yum -y install iptables-services
            sar_call
        fi
    
        # start iptables and ip6tables services
        systemctl start iptables
        systemctl start ip6tables
        systemctl enable iptables
        systemctl enable ip6tables
    else
        # leave firewalld enabled
        # disable CSF firewall instead
        service csf stop
        service lfd stop
        chkconfig csf off
        chkconfig lfd off

        # as CSF Firewall is disabled
        # need to setup firewalld permanent
        # services for default public zone
        firewall-cmd --permanent --zone=public --add-service=dns
        firewall-cmd --permanent --zone=public --add-service=ftp
        firewall-cmd --permanent --zone=public --add-service=http
        firewall-cmd --permanent --zone=public --add-service=https
        firewall-cmd --permanent --zone=public --add-service=imaps
        firewall-cmd --permanent --zone=public --add-service=mysql
        firewall-cmd --permanent --zone=public --add-service=pop3s
        firewall-cmd --permanent --zone=public --add-service=smtp
        firewall-cmd --permanent --zone=public --add-service=openvpn
        # firewall-cmd --permanent --zone=public --add-service=nfs

        # firewall-cmd --reload
        systemctl restart firewalld
        firewall-cmd --zone=public --list-services

        # custom ports allowed if detected SSHD default port is not 22, ensure the custom SSHD port
        # number is whitelisted by firewalld
        FWDDETECTED_PORT=$(awk '/^Port / {print $2}' /etc/ssh/sshd_config)
        if [[ "$FWDDETECTED_PORT" = '22' ]]; then
          FIREWALLD_PORTS='8080 8888 81 9418 30001-50011'
        else
          FIREWALLD_PORTS="$FWDDETECTED_PORT 8080 8888 81 9418 30001-50011"
        fi

        for fp in $FIREWALLD_PORTS
          do
            firewall-cmd --permanent --zone=public --add-port=${fp}/tcp
        done

        firewall-cmd --reload
        firewall-cmd --zone=public --list-ports
    fi
fi

    if [ -f /usr/local/src/centminmod/tools/csfcf.sh ]; then
        /usr/local/src/centminmod/tools/csfcf.sh auto
    fi

    echo "*************************************************"
    cecho "* CSF firewall installed " $boldgreen
    echo "*************************************************"
fi

}