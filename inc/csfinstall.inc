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

csf_maxmind_rego() {
    if [ -f /etc/csf/csf.conf ]; then
        # The Centmin Mod CSF Firewall fork supports CC_SRC="1" (MaxMind CSV
        # format) with an EMPTY MM_LICENSE_KEY by falling back to the Centmin Mod
        # GeoLite2 mirror at https://mxmind.centminmod.com/GeoLite2-{Country,City,ASN}-CSV.zip
        # so Centmin Mod no longer injects the shared MaxMind API key into
        # /etc/csf/csf.conf. Only two keys are ever written:
        #   1. a personal MaxMind key the end user set via MM_LICENSE_KEY in
        #      /etc/centminmod/custom_config.inc
        #   2. an existing non-empty key already in /etc/csf/csf.conf that Centmin
        #      Mod did not put there - that is always left untouched
        # https://blog.maxmind.com/2019/12/18/significant-changes-to-accessing-and-using-geolite2-databases/

        # personal MaxMind API key from Centmin Mod persistent config file
        if [ ! -f /etc/centminmod/custom_config.inc ]; then
            CHECK_CMM_MM_LICENSE_KEY=''
        else
            CHECK_CMM_MM_LICENSE_KEY=$(awk -F '=' '/^[[:space:]]*MM_LICENSE_KEY=/ {print $2}' /etc/centminmod/custom_config.inc | sed -e 's| ||g' | sed -e 's|"||g' -e "s|'||g")
        fi
        # currently configured MaxMind API key in CSF Firewall config file
        CHECK_CSF_MM_LICENSE_KEY=$(awk -F '=' '/^[[:space:]]*MM_LICENSE_KEY[[:space:]]*=/ {print $2}' /etc/csf/csf.conf | sed -e 's| ||g' | sed -e 's|"||g' -e "s|'||g" | head -n1)

        # only the Centmin Mod CSF Firewall fork build that carries the mirror
        # fallback (16.33+) can run CC_SRC="1" with an empty MM_LICENSE_KEY. On an
        # older CSF build keep the previous behaviour - leave the key alone and
        # use CC_SRC="2" - so country blocking never silently stops working.
        CSF_MIRROR_CAPABLE='n'
        if grep -q 'mxmind.centminmod.com' /usr/local/csf/lib/ConfigServer/Config.pm 2>/dev/null; then
            CSF_MIRROR_CAPABLE='y'
        fi

        CSF_MAXMIND_CHANGED='n'

        if [[ "$CHECK_CMM_MM_LICENSE_KEY" && "$CHECK_CSF_MM_LICENSE_KEY" != "$CHECK_CMM_MM_LICENSE_KEY" ]]; then
            # end user set their own personal MaxMind API key in
            # /etc/centminmod/custom_config.inc so keep /etc/csf/csf.conf in sync
            csf --profile backup cmm-before-maxmind-rego >/dev/null 2>&1
            sed -i "s|^MM_LICENSE_KEY = .*|MM_LICENSE_KEY = \"${CHECK_CMM_MM_LICENSE_KEY}\"|" /etc/csf/csf.conf
            csf --profile backup cmm-after-maxmind-rego >/dev/null 2>&1
            CSF_MAXMIND_CHANGED='y'
        elif [[ "$CSF_MIRROR_CAPABLE" = [yY] && ! "$CHECK_CMM_MM_LICENSE_KEY" && "$CHECK_CSF_MM_LICENSE_KEY" && "$GET_CMM_MM_LICENSE_KEY" && "$CHECK_CSF_MM_LICENSE_KEY" = "$GET_CMM_MM_LICENSE_KEY" ]]; then
            # migration: /etc/csf/csf.conf still holds the shared Centmin Mod
            # MaxMind API key that older Centmin Mod releases injected, and the end
            # user has no personal key of their own, so clear it and let the CSF
            # Firewall fork fall back to the Centmin Mod GeoLite2 CSV mirror.
            # Only ever clears a key that exactly matches the known shared key -
            # an unknown non-empty key is never touched.
            csf --profile backup cmm-before-maxmind-sharedkey-clear >/dev/null 2>&1
            sed -i "s|^MM_LICENSE_KEY = .*|MM_LICENSE_KEY = \"\"|" /etc/csf/csf.conf
            csf --profile backup cmm-after-maxmind-sharedkey-clear >/dev/null 2>&1
            CSF_MAXMIND_CHANGED='y'
        fi
        # every other state is deliberately left alone:
        #  - personal key already in sync with /etc/csf/csf.conf
        #  - unknown non-empty key independently configured by the end user
        #  - already empty key, which uses the CSF mirror fallback

        # CC_SRC="1" MaxMind CSV format is the Centmin Mod default and now works
        # with an empty MM_LICENSE_KEY. MM_CSF_SRC='n' opts back into CC_SRC="2"
        # db-ip.com / ipdeny country data. An older CSF build without the mirror
        # fallback also stays on CC_SRC="2" (the previous Centmin Mod default).
        if [[ "$MM_CSF_SRC" = [nN] || "$CSF_MIRROR_CAPABLE" = [nN] ]]; then
            if [[ ! "$(grep '^CC_SRC = "2"' /etc/csf/csf.conf)" ]]; then
                csf --profile backup before_cc_src_2_switch >/dev/null 2>&1
                sed -i 's|^CC_SRC = .*|CC_SRC = "2"|' /etc/csf/csf.conf
                csf --profile backup after_cc_src_2_switch >/dev/null 2>&1
                CSF_MAXMIND_CHANGED='y'
            fi
        else
            if [[ ! "$(grep '^CC_SRC = "1"' /etc/csf/csf.conf)" ]]; then
                csf --profile backup before_cc_src_1_switch >/dev/null 2>&1
                sed -i 's|^CC_SRC = .*|CC_SRC = "1"|' /etc/csf/csf.conf
                csf --profile backup after_cc_src_1_switch >/dev/null 2>&1
                CSF_MAXMIND_CHANGED='y'
            fi
        fi

        # normalise lfd GeoLite2 database refresh interval back to 14 days
        if [ ! "$(grep '^CC_INTERVAL = "14"' /etc/csf/csf.conf)" ]; then
            csf --profile backup cmm-before-maxmind-csf-cc-interval-reset >/dev/null 2>&1
            sed -i 's|^CC_INTERVAL = .*|CC_INTERVAL = "14"|' /etc/csf/csf.conf
            csf --profile backup cmm-after-maxmind-csf-cc-interval-reset >/dev/null 2>&1
            CSF_MAXMIND_CHANGED='y'
        fi

        if [[ "$CSF_MAXMIND_CHANGED" = [yY] && "$(csf -l >/dev/null 2>&1; echo $?)" -eq '0' ]]; then
            csf -ra >/dev/null 2>&1
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
    if [[ "$CENTOS_SEVEN" = '7' || "$CENTOS_EIGHT" -eq '8' || "$CENTOS_NINE" -eq '9' ]]; then
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
    # sed -i 's/20,21,22,25,53,853,80,110,143,443,465,587,993,995/20,21,22,25,53,853,80,110,111,143,161,443,465,587,993,995,1110,1186,1194,2049,81,9418,30001:50011/g' /etc/csf/csf.conf
    # non-FTP misc ports are always added; the pure-ftpd passive port range
    # 30001:50011 is only added when pure-ftpd is not disabled (PUREFTPD_DISABLED)
    # custom_config.inc is sourced in centmin.sh before csfinstalls runs so the
    # in-memory PUREFTPD_DISABLED value is valid here.
    CSF_ADDITIONAL_PORTS="1110,1186,1194,2049,81,9418"
    if [[ "$PUREFTPD_DISABLED" != [yY] ]]; then
      CSF_ADDITIONAL_PORTS="${CSF_ADDITIONAL_PORTS},30001:50011"
    fi
    # modify /etc/csf/csf.conf and save modifications /etc/csf/csf.conf.tmp
    awk -v additional="$CSF_ADDITIONAL_PORTS" '/^TCP_IN =/ {gsub("\"$", "," additional "\""); print; next} /^TCP6_IN =/ {gsub("\"$", "," additional "\""); print; next} {print}' /etc/csf/csf.conf > /etc/csf/csf.conf.tmp
    # compare /etc/csf/csf.conf vs /etc/csf/csf.conf.tmp via diff
    diff -u /etc/csf/csf.conf /etc/csf/csf.conf.tmp
    # overwrite existing csf.conf with modified
    mv /etc/csf/csf.conf.tmp /etc/csf/csf.conf

sed -i "s/TCP_OUT = \"/TCP_OUT = \"8080,2525,465,111,2049,1110,1194,9418,3306,/g" /etc/csf/csf.conf
sed -i "s/TCP6_OUT = \"/TCP6_OUT = \"8080,2525,465,3306,/g" /etc/csf/csf.conf
sed -i "s/UDP_IN = \"/UDP_IN = \"67,68,111,2049,1110,33434:33534,/g" /etc/csf/csf.conf
sed -i "s/UDP_OUT = \"/UDP_OUT = \"67,68,111,2049,1110,33434:33534,443,/g" /etc/csf/csf.conf
sed -i "s/DROP_NOLOG = \"67,68,/DROP_NOLOG = \"/g" /etc/csf/csf.conf

# ensure HTTP/3 UDP port 443 is open
if [[ "$NGINX_QUIC_SUPPORT" = [yY] ]]; then
  if [[ ! "$(grep 'UDP_IN =' /etc/csf/csf.conf | grep -o '443')" ]]; then
    csf --profile backup http3_udp_in
    # csf --profile list
    sed -i "s/UDP_IN = \"/UDP_IN = \"443,/g" /etc/csf/csf.conf
    grep 'UDP_IN =' /etc/csf/csf.conf | grep -o '443'
    # csf -ra >/dev/null 2>&1
  fi
  if [[ ! "$(grep 'UDP_OUT =' /etc/csf/csf.conf | grep -o '443')" ]]; then
    csf --profile backup http3_udp_out
    # csf --profile list
    sed -i "s/UDP_OUT = \"/UDP_OUT = \"443,/g" /etc/csf/csf.conf
    grep 'UDP_OUT =' /etc/csf/csf.conf | grep -o '443'
    # csf -ra >/dev/null 2>&1
  fi
  if [[ ! "$(grep 'UDP6_IN =' /etc/csf/csf.conf | grep -o '443')" ]]; then
    csf --profile backup http3_udp6_in
    # csf --profile list
    sed -i "s/UDP6_IN = \"/UDP6_IN = \"443,/g" /etc/csf/csf.conf
    grep 'UDP6_IN =' /etc/csf/csf.conf | grep -o '443'
    # csf -ra >/dev/null 2>&1
  fi
  if [[ ! "$(grep 'UDP6_OUT =' /etc/csf/csf.conf | grep -o '443')" ]]; then
    csf --profile backup http3_udp6_out
    # csf --profile list
    sed -i "s/UDP6_OUT = \"/UDP6_OUT = \"443,/g" /etc/csf/csf.conf
    grep 'UDP6_OUT =' /etc/csf/csf.conf | grep -o '443'
    # csf -ra >/dev/null 2>&1
  fi
fi

    egrep '^UDP_|^TCP_|^DROP_NOLOG' /etc/csf/csf.conf
    
    echo "Disabling CSF Testing mode (activates firewall)..."
    sed -i 's/TESTING = "1"/TESTING = "0"/g' /etc/csf/csf.conf

csf_maxmind_rego
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
if [[ "$CENTOS_SEVEN" = '7' || "$CENTOS_EIGHT" -eq '8' || "$CENTOS_NINE" -eq '9' ]]; then
    if [[ "$FIREWALLD_DISABLE" = [yY] ]]; then
        # disable firewalld
        if [[ "$CENTOS_EIGHT" = '8'|| "$CENTOS_NINE" -eq '9' ]]; then
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