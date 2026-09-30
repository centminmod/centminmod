luajitinstall() {

if [[ "$ORESTY_LUANGINX" = [yY] ]]; then
    # LuaJIT 2.0.x release tarballs are no longer published upstream and
    # lua-resty-core (required by lua-nginx-module) needs LuaJIT 2.1
    if [[ "$LUAJIT_GITINSTALL" != [yY] ]]; then
        cecho "LUAJIT_GITINSTALL='${LUAJIT_GITINSTALL}' is no longer supported, using LUAJIT_GITINSTALL='y'" $boldyellow
        cecho "LuaJIT ${ORESTY_LUAGITVER} is unavailable upstream and lua-resty-core requires LuaJIT 2.1" $boldyellow
        LUAJIT_GITINSTALL='y'
    fi
    echo "*************************************************"
    cecho "* LuaJIT-${LUAJIT_GITINSTALLVER} install" $boldgreen
    echo "*************************************************"
    if [[ "$LUAJIT_GITINSTALL" = [yY] ]]; then
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

        # make install names the binary luajit-$(VERSION), which luajit -v
        # reports for both rolling (RELVER from git) and older beta releases
        LUAJIT_BINVER=$(./src/luajit -v | awk '{print $2}')

        echo "luajit-${LUAJIT_BINVER}"

        if [ -x "/usr/local/nginx-dep/bin/luajit-${LUAJIT_BINVER}" ]; then
          rm -rf /usr/local/nginx-dep/bin/luajit
          ln -sf /usr/local/nginx-dep/bin/luajit-${LUAJIT_BINVER} /usr/local/nginx-dep/bin/luajit
        fi
    
        export LUAJIT_LIB='/usr/local/nginx-dep/lib'
        LUAJIT_GITVEREXPORT=$(echo ${LUAJIT_GITINSTALLVER} | sed -e 's|-agentzh||')
        export LUAJIT_INC="/usr/local/nginx-dep/include/luajit-${LUAJIT_GITVEREXPORT}"
    fi
fi

}