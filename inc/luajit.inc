luajitinstall() {

if [[ "$ORESTY_LUANGINX" = [yY] ]]; then
    echo "*************************************************"
    cecho "* LuaJIT-${ORESTY_LUAGITVER} install" $boldgreen
    echo "*************************************************"
    if [[ "$LUAJIT_GITINSTALL" = [nN] ]]; then
        cd $DIR_TMP
        cd LuaJIT-${ORESTY_LUAGITVER}
        if [[ "$INITIALINSTALL" != [yY] ]]; then
            make clean
        fi
        if [[ "$(uname -m)" = 'x86_64' ]]; then
            # enable GC64 mode in LuaJIT builds on 64bit systems
            # https://blog.openresty.com/en/luajit-gc64-mode/
            make${MAKETHREADS} XCFLAGS='-DLUAJIT_ENABLE_GC64' PREFIX='/usr/local/nginx-dep'
        else
            make${MAKETHREADS} PREFIX='/usr/local/nginx-dep'
        fi
        sar_call
        make install PREFIX='/usr/local/nginx-dep'
        sar_call
    
        export LUAJIT_LIB='/usr/local/nginx-dep/lib'
        export LUAJIT_INC='/usr/local/nginx-dep/include/luajit-2.0'
    elif [[ "$LUAJIT_GITINSTALL" = [yY] ]]; then
        cd $DIR_TMP
        if [[ -d "LuaJIT-${LUAJIT_GITINSTALLVER}" && -d "LuaJIT-${LUAJIT_GITINSTALLVER}/.git" ]]; then            
            cd LuaJIT-${LUAJIT_GITINSTALLVER}
            git stash
            git pull
            git log -3
            sar_call
        elif [[ -d "LuaJIT-${LUAJIT_GITINSTALLVER}" && ! -d "LuaJIT-${LUAJIT_GITINSTALLVER}/.git" ]]; then
            rm -rf LuaJIT-*
            time git clone https://github.com/openresty/luajit2 -b v${LUAJIT_GITINSTALLVER} LuaJIT-${LUAJIT_GITINSTALLVER}
            sar_call
            cd LuaJIT-${LUAJIT_GITINSTALLVER}
        else
            time git clone https://github.com/openresty/luajit2 -b v${LUAJIT_GITINSTALLVER} LuaJIT-${LUAJIT_GITINSTALLVER}
            sar_call
            cd LuaJIT-${LUAJIT_GITINSTALLVER}
        fi
        if [[ "$INITIALINSTALL" != [yY] ]]; then
            make clean
        fi
        if [[ "$(uname -m)" = 'x86_64' ]]; then
            # enable GC64 mode in LuaJIT builds on 64bit systems
            # https://blog.openresty.com/en/luajit-gc64-mode/
            make${MAKETHREADS} XCFLAGS='-DLUAJIT_ENABLE_GC64' PREFIX='/usr/local/nginx-dep'
        else
            make${MAKETHREADS} PREFIX='/usr/local/nginx-dep'
        fi
        sar_call
        make install PREFIX='/usr/local/nginx-dep'
        sar_call

        MAJVER=$(awk -F "=  " '/MAJVER=  / {print $2}' Makefile)
        MINVER=$(awk -F "=  " '/MINVER=  / {print $2}' Makefile)
        RELVER=$(awk -F "=  " '/RELVER=  / {print $2}' Makefile)
        PREREL=$(awk -F "=  " '/PREREL=  / {print $2}' Makefile)

        echo "luijit-${MAJVER}.${MINVER}.${RELVER}${PREREL}"

        rm -rf /usr/local/nginx-dep/bin/luajit        
        ln -sf /usr/local/nginx-dep/bin/luajit-${MAJVER}.${MINVER}.${RELVER}${PREREL} /usr/local/nginx-dep/bin/luajit
    
        export LUAJIT_LIB='/usr/local/nginx-dep/lib'
        LUAJIT_GITVEREXPORT=$(echo ${LUAJIT_GITINSTALLVER} | sed -e 's|-agentzh||')
        export LUAJIT_INC="/usr/local/nginx-dep/include/luajit-${LUAJIT_GITVEREXPORT}"
    fi
fi

}