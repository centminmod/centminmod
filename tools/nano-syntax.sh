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
    echo "To properly configure the right colors for your SSH client,"
    echo "script needs to know your configured text color in your SSH"
    echo "client. If your SSH client configured text color is other"
    echo "than black text, you need to specify what color text you want"
    echo
    echo "Choose your desired text color from the following options"
    echo "black, red, green, yellow, blue, magenta, cyan or white"
    echo "i.e. if you have white text on black background, choose"
    echo "white text below"
    echo
    read -ep "Which color do you want to set for text in syntax highlighting ? " color_opt
    echo
    git clone --depth=1 https://github.com/centminmod/nanorc
    if [ -d nanorc ]; then
        cd nanorc
        make install-global TEXT=$color_opt
    
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
                # if [[ "$(grep "$n" /etc/nanorc | grep 'nginx.nanorc')" ]] ; then
                #     echo "disable $n syntax highlighting"
                #     sed -i "s|^include \"\/usr\/local\/share\/nano\/$n|#include \"\/usr\/local\/share\/nano\/$n|"  /etc/nanorc
                # fi
                if [[ "$(grep "$n" /etc/nanorc | grep 'nginx.nanorc')" ]] ; then
                    echo "re-enable $n syntax highlighting"
                    sed -i "s|^#include \"\/usr\/local\/share\/nano\/$n|include \"\/usr\/local\/share\/nano\/$n|"  /etc/nanorc
                fi
            done
        fi
    fi
    echo "done..."
}

enable_syntax