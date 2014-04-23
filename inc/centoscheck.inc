function checkcentosver {

VERCOMPARE=`expr $CENTOSVER \> $TESTEDCENTOSVER`

if [[ "$VERCOMPARE" = "1"  ]]; then

	echo "-----------------------------------------------------"
        echo "Detected CentOS version greater than > CentOS $TESTEDCENTOSVER"
        echo "CentOS $TESTEDCENTOSVER is the latest tested version for Centmin Mod"
        echo "No guarantee 100% working install with your CentOS $CENTOSVER"
	echo "-----------------------------------------------------"
        echo ""
        read -ep "Do you still want to continue with install ? [y/n]: " centoscheckcont

        if [[ "$centoscheckcont" != [yY] ]]; then

        echo ""
        echo "Regularly check centminmod.com for news on new CentOS versions support"
        echo "aborting script..."
        exit

        fi

else

        centoscheckcont='y'

fi

if [ ! -f /etc/ld.so.preload ]; then
touch /etc/ld.so.preload
fi

MACHINE_TYPE=`uname -m` # Used to detect if OS is 64bit or not.

    if [ ${MACHINE_TYPE} == 'x86_64' ];
    then
        ARCH='x86_64'
    else
        ARCH='i386'
    fi

# Set LIBDIR
if [ ${ARCH} == 'x86_64' ];
then
    LIBDIR='lib64'
else
    LIBDIR='lib'
fi

if [ -f /usr/${LIBDIR}/sa/sa2 ]; then

	# SAR extend log from 7 to 21 days
	SARDAYCHECK=`awk -F "=" '/^HISTORY/ { print $2 }' /usr/${LIBDIR}/sa/sa2`

	if [ "$SARDAYCHECK" == '7' ]; then
	sed -i 's/HISTORY=7/HISTORY=21/g' /usr/${LIBDIR}/sa/sa2
	fi
fi

if [[ -f /root/.bash_profile ]]; then
    NANOCHECK=`grep nano /root/.bash_profile`
    if [ -z "$NANOCHECK" ]; then
    sed -i 's/export PATH/export PATH\nexport EDITOR=nano/g' /root/.bash_profile
    fi
else
    cat > /root/.bash_profile <<EOF
# .bash_profile

# Get the aliases and functions
if [ -f ~/.bashrc ]; then
        . ~/.bashrc
fi

# User specific environment and startup programs

PATH=\$PATH:\$HOME/bin

export PATH
export EDITOR=nano
export HISTTIMEFORMAT="%a %h %d – %r "
EOF

# cat /root/.bash_profile
fi

if [ -f /root/.screenrc ]; then
	DEFSCROLLCHECK=`grep defscrollback /root/.screenrc`
	if [ -z "$DEFSCROLLCHECK" ]; then
	echo "defscrollback 200000" >> /root/.screenrc
	fi
else
	echo "defscrollback 200000" >> /root/.screenrc
fi

if [ ! -f /etc/centminmod-release ];then
ccacheinstall
fi

if [ -f /etc/centminmod-release ];then
	if [[ "$SCRIPT_VERSION" != "$(cat /etc/centminmod-release >/dev/null)" ]]; then
	setupmotd
	fi
else
	setupmotd
fi

sshdtweaks

countcpus

}