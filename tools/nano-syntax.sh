#!/bin/bash
######################################################
# enable and extend nano editor syntax highlighting
# written by George Liu centminmod.com
######################################################
# variables
#############
DT=$(date +"%d%m%y-%H%M%S")
CENTMINLOGDIR='/root/centminlogs'

DIR_TMP=/svr-setup
######################################################
CENTOSVER=$(awk '{ print $3 }' /etc/redhat-release)

if [ "$CENTOSVER" == 'release' ]; then
    CENTOSVER=$(awk '{ print $4 }' /etc/redhat-release | cut -d . -f1,2)
    if [[ "$(cat /etc/redhat-release | awk '{ print $4 }' | cut -d . -f1)" = '7' ]]; then
        CENTOS_SEVEN='7'
    fi
fi

if [[ "$(cat /etc/redhat-release | awk '{ print $3 }' | cut -d . -f1)" = '6' ]]; then
    CENTOS_SIX='6'
fi

if [ "$CENTOSVER" == 'Enterprise' ]; then
    CENTOSVER=$(cat /etc/redhat-release | awk '{ print $7 }')
    OLS='y'
fi

if [ -f "/etc/centminmod/custom_config.inc" ]; then
  # default is at /etc/centminmod/custom_config.inc
  . "/etc/centminmod/custom_config.inc"
fi

######################################################
enable_syntax() {
    cd $DIR_TMP
    rm -rf nanorc
    echo
    echo "Setup Extended Syntax Highlighting for nano editor"
    echo
    git clone --depth=1 https://github.com/centminmod/nanorc
    if [ -d nanorc ]; then
        cd nanorc
        make install-global
    
        if [[ -d /usr/local/share/nano && -f /etc/nanorc ]]; then
            NANORC_LIST='shell.nanorc nginx.nanorc go.nanorc ini.nanorc javascript.nanorc json.nanorc markdown.nanorc rpmspec.nanorc sql.nanorc systemd.nanorc yaml.nanorc yum.nanorc'
            if [[ -z "$(grep '#include "\/usr\/share\/nano\/sh.nanorc"' /etc/nanorc)" ]]; then
                sed -i "s|^include "\/usr\/share\/nano\/sh.nanorc"|#include "\/usr\/share\/nano\/sh.nanorc"|" /etc/nanorc
            fi
            for n in $NANORC_LIST; do
                if [[ -z "$(grep "$n" /etc/nanorc)" ]] ; then
                    echo "setup $n syntax highlighting"
                    echo -e "\n## $n" >> /etc/nanorc
                    echo "include \"/usr/local/share/nano/$n\"" >> /etc/nanorc
                fi
                if [[ "$(grep "$n" /etc/nanorc | grep 'nginx.nanorc')" ]] ; then
                    echo "disable $n syntax highlighting"
                    sed -i "s|^include \"\/usr\/local\/share\/nano\/$n|#include \"\/usr\/local\/share\/nano\/$n|"  /etc/nanorc
                fi
            done
        fi
    fi
    echo "done..."
}

enable_syntax