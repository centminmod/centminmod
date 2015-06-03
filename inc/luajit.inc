luajitinstall() {

if [[ "$ORESTY_LUANGINX" = [yY] ]]; then
    echo "*************************************************"
    cecho "* LuaJIT-${ORESTY_LUAGITVER} install" $boldgreen
    echo "*************************************************"

    if [[ "$LUAJIT_GITINSTALL" = [nN] ]]; then
        cd $DIR_TMP
        cd LuaJIT-${ORESTY_LUAGITVER}
        make clean
        make${MAKETHREADS}
        make install
    
        export LUAJIT_LIB='/usr/local/lib'
        export LUAJIT_INC='/usr/local/include/luajit-2.0'
    elif [[ "$LUAJIT_GITINSTALL" = [yY] ]]; then
        cd $DIR_TMP
        if [[ -d "LuaJIT-${LUAJIT_GITINSTALLVER}" && -d "LuaJIT-${LUAJIT_GITINSTALLVER}/.git" ]]; then            
            cd LuaJIT-${LUAJIT_GITINSTALLVER}
            git stash
            git pull
            git log -3
        elif [[ -d "LuaJIT-${LUAJIT_GITINSTALLVER}" && ! -d "LuaJIT-${LUAJIT_GITINSTALLVER}/.git" ]]; then
            rm -rf LuaJIT-*
            git clone http://luajit.org/git/luajit-2.0.git -b v${LUAJIT_GITINSTALLVER} LuaJIT-${LUAJIT_GITINSTALLVER}
            cd LuaJIT-${LUAJIT_GITINSTALLVER}
        else
            git clone http://luajit.org/git/luajit-2.0.git -b v${LUAJIT_GITINSTALLVER} LuaJIT-${LUAJIT_GITINSTALLVER}
            cd LuaJIT-${LUAJIT_GITINSTALLVER}
        fi
        make clean
        make${MAKETHREADS}
        make install

        MAJVER=$(awk -F "=  " '/MAJVER=  / {print $2}' Makefile)
        MINVER=$(awk -F "=  " '/MINVER=  / {print $2}' Makefile)
        RELVER=$(awk -F "=  " '/RELVER=  / {print $2}' Makefile)
        PREREL=$(awk -F "=  " '/PREREL=  / {print $2}' Makefile)

        echo "luijit-${MAJVER}.${MINVER}.${RELVER}${PREREL}"

        ln -sf luijit-${MAJVER}.${MINVER}.${RELVER}${PREREL} /usr/local/bin/luajit
    
        export LUAJIT_LIB='/usr/local/lib'
        export LUAJIT_INC="/usr/local/include/luajit-${LUAJIT_GITINSTALLVER}"
    fi
fi

}