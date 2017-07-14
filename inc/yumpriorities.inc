yumprioratrpms() {
if [ -f /etc/yum.repos.d/atrpms.repo ]; then
cp -p /etc/yum.repos.d/atrpms.repo /etc/yum.repos.d/atrpms.OLD
if [ -n "$(grep ^priority /etc/yum.repos.d/atrpms.repo)" ]
        then
                #echo priorities already set for atrpms.repo
PRIOREXISTS=1
        else
                echo "setting yum priorities for atrpms.repo"
                ex -s /etc/yum.repos.d/atrpms.repo << EOF
:/\[atrpms/ , /gpgkey/
:a
priority=90
.
:w
:/\[atrpms-debuginfo/ , /gpgkey/
:a
priority=90
.
:w
:q
EOF
fi
fi # repo file check

if [ -f /etc/yum.repos.d/atrpms-bleeding.repo ]; then
cp -p /etc/yum.repos.d/atrpms-bleeding.repo /etc/yum.repos.d/atrpms-bleeding.OLD
if [ -n "$(grep ^priority /etc/yum.repos.d/atrpms-bleeding.repo)" ]
        then
                #echo priorities already set for atrpms-bleeding.repo
PRIOREXISTS=1
        else
                echo "setting yum priorities for atrpms-bleeding.repo"
                ex -s /etc/yum.repos.d/atrpms-bleeding.repo << EFF
:/\[atrpms-bleeding/ , /gpgkey/
:a
priority=90
.
:w
:/\[atrpms-bleeding-debuginfo/ , /gpgkey/
:a
priority=90
.
:w
:q
EFF
fi
fi # repo file check

if [ -f /etc/yum.repos.d/atrpms-testing.repo ]; then
cp -p /etc/yum.repos.d/atrpms-testing.repo /etc/yum.repos.d/atrpms-testing.OLD
if [ -n "$(grep ^priority /etc/yum.repos.d/atrpms-testing.repo)" ]
        then
                #echo priorities already set for atrpms-testing.repo
PRIOREXISTS=1
        else
                echo "setting yum priorities for atrpms-testing.repo"
                ex -s /etc/yum.repos.d/atrpms-testing.repo << EFE
:/\[atrpms-testing/ , /gpgkey/
:a
priority=90
.
:w
:/\[atrpms-testing-debuginfo/ , /gpgkey/
:a
priority=90
.
:w
:q
EFE

cecho "*************************************************" $boldgreen
cecho "Fixing atrpms.repo YUM Priorities" $boldgreen
cecho "*************************************************" $boldgreen
echo "cat /etc/yum.repos.d/atrpms.repo"
echo "cat /etc/yum.repos.d/atrpms-bleeding.repo"
echo "cat /etc/yum.repos.d/atrpms-testing.repo"
cat /etc/yum.repos.d/atrpms.repo
cat /etc/yum.repos.d/atrpms-bleeding.repo
cat /etc/yum.repos.d/atrpms-testing.repo
echo ""
fi
fi # repo file check
}

yumprior() {

# check for Oracle Linux repo
if [[ -f /etc/yum.repos.d/public-yum-ol6.repo ]]; then
        OLSEXISTS='y'
fi

if [[ "$OLSEXISTS" = [yY] || "$OLS" = [yY] ]]; then
        #oracle linux repo priorities

        cp -p /etc/yum.repos.d/public-yum-ol6.repo /etc/yum.repos.d/public-yum-ol6.repo.OLD
        
        if [ -n "$(grep ^priority /etc/yum.repos.d/public-yum-ol6.repo)" ]
                then
                        #echo priorities already set for public-yum-ol6.repo
                PRIOREXISTS=1
                else
                        echo "setting yum priorities for public-yum-ol6.repo"
                ex -s /etc/yum.repos.d/public-yum-ol6.repo << EOF
:/\[ol6_latest/ , /gpgkey/
:a
priority=1
.
:w
:/\[ol6_addons/ , /gpgkey/
:a
priority=1
.
:w
:/\[ol6_ga_base/ , /gpgkey/
:a
priority=1
.
:w
:/\[ol6_u1_base/ , /gpgkey/
:a
priority=1
.
:w
:/\[ol6_u2_base/ , /gpgkey/
:a
priority=1
.
:w
:/\[ol6_u3_base/ , /gpgkey/
:a
priority=1
.
:w
:/\[ol6_u4_base/ , /gpgkey/
:a
priority=1
.
:w
:/\[ol6_u5_base/ , /gpgkey/
:a
priority=1
.
:w
:/\[ol6_UEKR3_latest/ , /gpgkey/
:a
priority=1
.
:w
:/\[ol6_UEK_latest/ , /gpgkey/
:a
priority=1
.
:w
:/\[ol6_UEK_base/ , /gpgkey/
:a
priority=1
.
:w
:/\[ol6_playground_latest/ , /gpgkey/
:a
priority=1
.
:w
:/\[ol6_MySQL/ , /gpgkey/
:a
priority=1
.
:w
:/\[ol6_gdm_multiseat/ , /gpgkey/
:a
priority=1
.
:w
:/\[ol6_ofed_UEK/ , /gpgkey/
:a
priority=1
.
:w
:/\[ol6_MySQL56/ , /gpgkey/
:a
priority=1
.
:w
:/\[ol6_spacewalk20_server/ , /gpgkey/
:a
priority=1
.
:w
:/\[ol6_spacewalk20_client/ , /gpgkey/
:a
priority=1
.
:w
:q
EOF

        cecho "*************************************************" $boldgreen
        cecho "Fixing public-yum-ol6.repo YUM Priorities" $boldgreen
        cecho "*************************************************" $boldgreen
        cat /etc/yum.repos.d/public-yum-ol6.repo
        echo ""
        fi

else
        # check if CentOS-Base.repo is actually used
        if [[ "$(grep -q '^#\[base\]$' /etc/yum.repos.d/CentOS-Base.repo; echo $?)" != '0' ]]; then
        #centos repo priorities
        cp -p /etc/yum.repos.d/CentOS-Base.repo /etc/yum.repos.d/CentOS-Base.OLD
        
        if [ -n "$(grep ^priority /etc/yum.repos.d/CentOS-Base.repo)" ]
                then
                        #echo priorities already set for CentOS-Base.repo
	       PRIOREXISTS=1
                else
                        echo "setting yum priorities for CentOS-Base.repo"
                ex -s /etc/yum.repos.d/CentOS-Base.repo << EOF
:/\[base/ , /gpgkey/
:a
priority=1
.
:w
:/\[updates/ , /gpgkey/
:a
priority=1
.
:w
:/\[addons/ , /gpgkey/
:a
priority=1
.
:/\[extras/ , /gpgkey/
:a
priority=1
.
:/\[centosplus/ , /gpgkey/
:a
priority=2
.
:/\[contrib/ , /gpgkey/
:a
priority=2
.
:w
:q
EOF
        
        cecho "*************************************************" $boldgreen
        cecho "Fixing CentOS-Base.repo YUM Priorities" $boldgreen
        cecho "*************************************************" $boldgreen
        cat /etc/yum.repos.d/CentOS-Base.repo
        echo ""
        fi
        fi # check if CentOS-Base.repo is actually used
        
        if [ -f /etc/yum.repos.d/CentOS-CR.repo ]; then
        cp -p /etc/yum.repos.d/CentOS-CR.repo /etc/yum.repos.d/CentOS-CR.OLD
        if [ -n "$(grep ^priority /etc/yum.repos.d/CentOS-CR.repo)" ]
                then
                        #echo priorities already set for CentOS-CR.repo
	       PRIOREXISTS=1
                else
                        echo "setting yum priorities for CentOS-CR.repo"
                ex -s /etc/yum.repos.d/CentOS-CR.repo << EOF
:/\[cr/ , /gpgkey/
:a
priority=1
.
:w
:q
EOF
        
        cecho "*************************************************" $boldgreen
        cecho "Fixing CentOS-CR.repo YUM Priorities" $boldgreen
        cecho "*************************************************" $boldgreen
        cat /etc/yum.repos.d/CentOS-CR.repo
        echo ""
        fi
        
        fi # CentOS-CR.repo file check

fi # OLSEXISTS CHECK

} # yumprior
        
yumpriorepel() {
if [ -f /etc/yum.repos.d/epel.repo ]; then
cp -p /etc/yum.repos.d/epel.repo /etc/yum.repos.d/epel.OLD
if [ -n "$(grep ^priority /etc/yum.repos.d/epel.repo)" ]
        then
                #echo priorities already set for epel.repo
PRIOREXISTS=1
        else
                echo "setting yum priorities for epel.repo"
                ex -s /etc/yum.repos.d/epel.repo << EOF
:/\[epel\]/ , /gpgkey/
:a
priority=3
exclude=varnish varnish-libs galera nodejs nginx mongodb redis*
.
:w
:q
EOF

cecho "*************************************************" $boldgreen
cecho "Fixing epel.repo YUM Priorities" $boldgreen
cecho "*************************************************" $boldgreen
cat /etc/yum.repos.d/epel.repo
echo ""
fi
fi # repo file check
}


yumpriorrpmforge() {
  forgefix
if [ -f /etc/yum.repos.d/rpmforge.repo ]; then
cp -p /etc/yum.repos.d/rpmforge.repo /etc/yum.repos.d/rpmforge.OLD
    if [[ "$CENTOS_SEVEN" = '7' && "$DNF_ENABLE" = [yY] ]]; then
        sed -i 's|enabled .*|enabled = 0|g' /etc/yum.repos.d/rpmforge.repo
    fi
if [ -n "$(grep ^priority /etc/yum.repos.d/rpmforge.repo)" ]
        then
                #echo priorities already set for rpmforge.repo
PRIOREXISTS=1
        else
                echo "setting yum priorities for rpmforge.repo"
                ex -s /etc/yum.repos.d/rpmforge.repo << EOF
:/\[rpmforge\]/ , /gpgcheck/
:a
priority=5
exclude=pure-ftpd optipng clamav* clamd
.
:w
:q
EOF

cecho "*************************************************" $boldgreen
cecho "Fixing rpmforge.repo YUM Priorities" $boldgreen
cecho "*************************************************" $boldgreen
cat /etc/yum.repos.d/rpmforge.repo
echo ""
fi
fi # repo file check
}

yumpriorcentalt() {
if [ -f /etc/yum.repos.d/centalt.repo ]; then
cp -p /etc/yum.repos.d/centalt.repo /etc/yum.repos.d/centalt.OLD
if [ -n "$(grep ^priority /etc/yum.repos.d/centalt.repo)" ]
        then
                #echo priorities already set for centalt.repo
PRIOREXISTS=1
        else
                echo "setting yum priorities for centalt.repo"
                ex -s /etc/yum.repos.d/centalt.repo << EOF
:/\[CentALT\]/ , /gpgkey/
:a
priority=9
.
:w
:q
EOF

cecho "*************************************************" $boldgreen
cecho "Fixing centalt.repo YUM Priorities" $boldgreen
cecho "*************************************************" $boldgreen
sed -i 's/enabled=1/enabled=0/g' /etc/yum.repos.d/centalt.repo
cat /etc/yum.repos.d/centalt.repo
echo ""
fi
fi # repo file check
}

yumpriorpercona() {
if [ -f /etc/yum.repos.d/percona-release.repo ]; then
cp -p /etc/yum.repos.d/percona-release.repo /etc/yum.repos.d/percona-release.OLD
if [ -n "$(grep ^priority /etc/yum.repos.d/percona-release.repo)" ]
        then
                #echo priorities already set for percona-release.repo
PRIOREXISTS=1
        else
                echo "setting yum priorities for percona-release.repo"
                ex -s /etc/yum.repos.d/percona-release.repo << EOF
:/\[percona-release-\$basearch\]/ , /gpgcheck/
:a
priority=11
exclude=Percona-SQL* Percona-Server* Percona-XtraDB-Cluster*
.
:w
:q
EOF

sed -i 's/enabled = 1/enabled = 0/g' /etc/yum.repos.d/percona-release.repo

cecho "*************************************************" $boldgreen
cecho "Fixing percona-release.repo YUM Priorities" $boldgreen
cecho "*************************************************" $boldgreen
cat /etc/yum.repos.d/percona-release.repo
echo ""
fi
fi # repo file check
}

yumpriorvarnish() {
if [ -f /etc/yum.repos.d/varnish-4.1.repo ]; then
cp -p /etc/yum.repos.d/varnish-4.1.repo /etc/yum.repos.d/varnish-4.1.OLD
if [ -n "$(grep ^priority /etc/yum.repos.d/varnish-4.1.repo)" ]
        then
                #echo priorities already set for varnish-4.1.repo
PRIOREXISTS=1
        else
                echo "setting yum priorities for varnish-4.1.repo"
                ex -s /etc/yum.repos.d/varnish-4.1.repo << EOF
:/\[varnish-4.1\]/ , /gpgcheck/
:a
priority=2
.
:w
:q
EOF

# sed -i 's/enabled=1/enabled=0/g' /etc/yum.repos.d/varnish-4.1.repo

cecho "*************************************************" $boldgreen
cecho "Fixing varnish-4.1.repo YUM Priorities" $boldgreen
cecho "*************************************************" $boldgreen
cat /etc/yum.repos.d/varnish-4.1.repo
echo ""
fi
fi # repo file check
}

yumprioraxivo() {
if [[ "$AXIVOREPO_DISABLE" = [nN] ]]; then
if [ -f /etc/yum.repos.d/axivo.repo ]; then
        cp -p /etc/yum.repos.d/axivo.repo /etc/yum.repos.d/axivo.OLD
        if [ -n "$(grep ^priority /etc/yum.repos.d/axivo.repo)" ]
                then
                        #echo priorities already set for axivo.repo
        PRIOREXISTS=1
                else
                        echo "setting yum priorities for axivo.repo"
ex -s /etc/yum.repos.d/axivo.repo << EOF
:/\[axivo\]/ , /gpgkey/
:a
priority=13
exclude=nginx* php* mariadb* memcached* xtrabackup
.
:w
:q
EOF
        
        cecho "*************************************************" $boldgreen
        cecho "Fixing axivo.repo YUM Priorities" $boldgreen
        cecho "*************************************************" $boldgreen
        cat /etc/yum.repos.d/axivo.repo
        echo ""
        fi
fi # /etc/yum.repos.d/axivo.repo
fi # AXIVOREPO_DISABLE
}

yumpriorremi() {
if [[ "$REMIREPO_DISABLE" = [nN] ]]; then
if [ -f /etc/yum.repos.d/remi.repo ]; then
        if [ -f /etc/yum.repos.d/remi-safe.repo ]; then
                sed -i 's|enabled=1|enabled=0|' /etc/yum.repos.d/remi-safe.repo
        fi
        cp -p /etc/yum.repos.d/remi.repo /etc/yum.repos.d/remi.OLD
        if [ -n "$(grep ^priority /etc/yum.repos.d/remi.repo)" ]
                then
                        #echo priorities already set for remi.repo
        PRIOREXISTS=1
                else
                        echo "setting yum priorities for remi.repo"
ex -s /etc/yum.repos.d/remi.repo << EOF
:/\[remi/ , /gpgkey/
:a
priority=9
exclude=php* mysql*
.
:w
:q
EOF
        
        cecho "*************************************************" $boldgreen
        cecho "Fixing remi.repo YUM Priorities" $boldgreen
        cecho "*************************************************" $boldgreen
        cat /etc/yum.repos.d/remi.repo
        echo ""
        fi
fi # /etc/yum.repos.d/remi.repo
fi # REMIREPO_DISABLE
}