#!/usr/bin/env python3
"""Optional developer checks requiring Python 3.6+; run test-wordpress-config.sh.

Checks templates and PHP preflight without installing or changing a site.
Centmin Mod installation and auto-updates never invoke this test suite.
"""
import os
from pathlib import Path
import re
import shlex
import shutil
import subprocess
import sys
import tempfile

if not __debug__:
    sys.exit("WordPress tests require assertions; run test-wordpress-config.sh without Python -O.")

ROOT = Path(sys.argv[1]).resolve() if len(sys.argv) > 1 else Path(__file__).resolve().parents[1]
GENERATORS = (
    "inc/wpsetup.inc",
    "inc/wpsetup-fastcgi-cache.inc",
    "tools/regen_wpsecure.sh",
    "tools/nvwp.sh",
    "tools/wp-cache-enabler-generate.sh",
)
ENV = {"PATH": os.environ["PATH"], "LC_ALL": "C"}


def shell(code, **env):
    return subprocess.run(["bash"], input=code, encoding="utf-8", stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                          env={**ENV, **env})


def heredoc(source, filename, tag):
    match = re.search(r"^cat > [^\n]*" + re.escape(filename) + r"[^\n]*<<" + tag
                      + r"\n(.*?)\n" + tag + r"(?:\n|$)", source, re.M | re.S)
    assert match, filename
    return match[1] + "\n"


def render(body, prefix="", **variables):
    # Expand the actual template with Bash, but never execute an installer or
    # allow template command substitution to affect the host running this test.
    assert "$(" not in body and "`" not in body
    result = shell("cat <<CMM_TEST_EOF\n" + body + "CMM_TEST_EOF\n",
                   **{"WPSUBDIR": prefix, "vhostname": "wp-test.invalid",
                      "fastcgicache_name": "wptest", **variables})
    assert result.returncode == 0, result.stderr
    return result.stdout


def check_templates():
    sources = {name: (ROOT / name).read_text(encoding="utf-8") for name in GENERATORS}
    for name in (*GENERATORS, "inc/cpcheck.inc"):
        subprocess.run(["bash", "-n", str(ROOT / name)], check=True)
    for prefix in ("", "/blog", "/sites/news"):
        blocks = []
        for name, source in sources.items():
            output = render(heredoc(source, "wpsecure_", "EEF"), prefix)
            block = re.search(r"# WordPress OAuth discovery start\n.*?"
                              r"# WordPress OAuth discovery end\n", output, re.S).group()
            blocks.append(block)
            assert block.count("location = " + prefix + "/.well-known/oauth-") == 2, name
            assert block.count("try_files $uri " + prefix + "/index.php?$args;") == 2, name
            assert block.count("include /usr/local/nginx/conf/503include-only.conf;") == 2, name
            assert output.count("location ~ /.well-known/acme-challenge/(.*) {") == 1, name
            assert "${WPSUBDIR}" not in output and "\\$uri" not in output, name
        assert len(set(blocks)) == 1, "Generators disagree about discovery routing"

    source = sources["inc/wpsetup-fastcgi-cache.inc"]
    cache_map = render(heredoc(source, "wpfastcgi_cache_map.conf", "FCD"))
    rest = re.search(r"^\s+~(\S*wp-json\S*)\s+1;", cache_map, re.M).group(1)
    oauth = re.search(r"^\s+~(\S*oauth-\S*)\s+1;", cache_map, re.M).group(1)
    for prefix in ("", "/blog", "/sites/news"):
        for suffix in ("", "/", "?context=edit", "/wp/v2/users/me"):
            assert re.search(rest, prefix + "/wp-json" + suffix)
        for endpoint in ("protected-resource", "authorization-server"):
            for suffix in ("", "?test=1"):
                assert re.search(oauth, prefix + "/.well-known/oauth-" + endpoint + suffix)
    for uri in ("/wp-json-other", "/public-page", "/?redirect=/wp-json/"):
        assert not re.search(rest, uri), uri
    for uri in ("/.well-known/acme-challenge/token", "/.well-known/oauth-protected-resource-extra",
                "/public-page", "/x?url=/.well-known/oauth-protected-resource"):
        assert not re.search(oauth, uri), uri

    php = render(heredoc(source, "php-fastcgicache.conf", "HFF"))
    assert 'fastcgi_cache_key       "$scheme$request_method$host$request_uri$normalized_encoding";' in php
    for directive in ("fastcgi_no_cache", "fastcgi_cache_bypass"):
        line = re.search(r"^" + directive + r"\s+([^;]+);", php, re.M).group(1)
        assert "$http_authorization" in line.split(), directive
        assert "$wpfcgi_nocacheuri" in line.split(), directive
    for name in GENERATORS[:2]:
        redis = render(heredoc(sources[name], "rediscache_", "XFF"))
        assert 'if ($request_uri ~ "' + oauth + '") {\n  set $skip_cache 1;\n}' in redis, name
        for filename, tag in (("php-rediscache.conf", "HFF"), ("php-rediscache-shortttl.conf", "HFI")):
            php = render(heredoc(sources[name], filename, tag))
            assert "srcache_fetch_skip $skip_cache;" in php and "srcache_store_skip $skip_cache;" in php
    assert "-subdir" not in sources["tools/regen_wpsecure.sh"]
    assert 'WPSUBDIR=""' in sources["tools/nvwp.sh"]
    assert 'WPSUBDIR=""' in sources["tools/wp-cache-enabler-generate.sh"]
    print("PASS: Bash syntax, five rendered generators, nested REST/discovery exclusions and cache gates")


def executable(path, body):
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text("#!/bin/bash\n" + body + "\n", encoding="utf-8")
    path.chmod(0o755)


def check_launcher():
    launcher = ROOT / "tools/test-wordpress-config.sh"
    bash, dirname = shutil.which("bash"), shutil.which("dirname")
    subprocess.run([bash, "-n", str(launcher)], check=True)
    cases = (
        ("missing Python", {}, 77, None),
        ("only old Python", {"python": False, "python3": False}, 77, None),
        ("python36 fallback", {"python3": False, "python36": True}, 0, "python36"),
        ("versioned fallback", {"python3": False, "python3.6": True}, 0, "python3.6"),
        ("venv Python", {"python": True}, 0, "python"),
        ("first compatible Python", {"python3": True, "python36": True}, 0, "python3"),
        ("test failure", {"python3": True}, 42, "python3"),
    )
    for label, interpreters, status, selected in cases:
        with tempfile.TemporaryDirectory(prefix="cmm-wp-launcher-") as directory:
            base = Path(directory)
            (base / "dirname").symlink_to(dirname)
            for name, compatible in interpreters.items():
                executable(base / name, 'if [[ "$2" = -c ]]; then exit ' + ("0" if compatible else "1")
                           + '; fi\nprintf "%s\\n" "${0##*/}" "$@" > "$TEST_ARGUMENTS"\nexit "$TEST_STATUS"')
            arguments = base / "arguments"
            result = subprocess.run([bash, str(launcher), "/repo with spaces"], encoding="utf-8",
                                    stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                                    env={"PATH": str(base), "TEST_ARGUMENTS": str(arguments),
                                         "TEST_STATUS": str(status), "PYTHONOPTIMIZE": "1"})
            assert result.returncode == status, (label, result.stdout, result.stderr)
            assert result.stdout == "", (label, result.stdout)
            if selected:
                assert result.stderr == "", (label, result.stderr)
                assert arguments.read_text(encoding="utf-8").splitlines() == [selected, "-E", str(launcher.with_suffix(".py")), "/repo with spaces"], label
            else:
                assert result.stderr.startswith("SKIP:") and not arguments.exists(), label
    print("PASS: optional test launcher skips unavailable Python, selects fallbacks and preserves test failures")


def check_php_preflight():
    source = (ROOT / "inc/wpsetup.inc").read_text(encoding="utf-8")
    helper = source.split("wpinstall() {", 1)[0]
    assert "wp_php_preflight || return 1" in source
    assert "wp_php_preflight || return 1" in (ROOT / "inc/wpsetup-fastcgi-cache.inc").read_text(encoding="utf-8")
    with tempfile.TemporaryDirectory(prefix="cmm-wp-php-") as directory:
        base = Path(directory)
        conf = base / "nginx"
        conf.mkdir()
        executable(base / "bin/php", 'printf "%s" "$CLI_VERSION"; exit "${CLI_STATUS:-0}"')
        executable(base / "bin/ss", 'printf "%s\\n" "$SS_OUTPUT"')
        executable(base / "bin/nginx", 'printf "%s\\n" "$NGINX_DUMP"; exit "${NGINX_STATUS:-0}"')
        executable(base / "bin/readlink", 'printf "%s\\n" "$FPM_EXE_TARGET"; exit "${READLINK_STATUS:-0}"')
        executable(base / "proc/4242/exe", 'printf "%s\\n" "$FPM_VERSION"; exit "${FPM_STATUS:-0}"')
        helper = helper.replace("/usr/local/nginx/conf/", str(conf) + "/").replace("/proc/", str(base / "proc") + "/")
        defaults = {
            "PATH": str(base / "bin") + os.pathsep + ENV["PATH"],
            "SCRIPT_DIR": str(ROOT),
            "NGINX_DUMP": "# configuration file " + str(conf / "default_phpupstream.conf") + ":",
            "CLI_VERSION": "80423", "FPM_VERSION": "PHP 8.4.23 (fpm-fcgi) (built: test)",
            "FPM_EXE_TARGET": "/usr/local/sbin/php-fpm",
            "SS_OUTPUT": 'LISTEN 0 128 127.0.0.1:9000 0.0.0.0:* users:(("php-fpm",pid=4242,fd=8))',
        }
        cases = (
            ("current CLI and FPM", True, {}, "127.0.0.1:9000;", "127.0.0.1:9000;"),
            ("legacy ss process tuple", True, {"SS_OUTPUT": 'LISTEN 0 128 127.0.0.1:9000 *:* users:(("php-fpm",4242,8))'}, "127.0.0.1:9000;", "127.0.0.1:9000;"),
            ("legacy ss wrong executable", False, {"SS_OUTPUT": 'LISTEN 0 128 127.0.0.1:9000 *:* users:(("php-fpm",4242,8))', "FPM_EXE_TARGET": "/usr/bin/custom-server"}, "127.0.0.1:9000;", "127.0.0.1:9000;"),
            ("legacy ss invalid PID", False, {"SS_OUTPUT": 'LISTEN 0 128 127.0.0.1:9000 *:* users:(("php-fpm",invalid,8))'}, "127.0.0.1:9000;", "127.0.0.1:9000;"),
            ("PHP 7.4 minimum", True, {"CLI_VERSION": "70400", "FPM_VERSION": "PHP 7.4.0 (fpm-fcgi)"}, "127.0.0.1:9000;", "127.0.0.1:9000;"),
            ("future PHP major", True, {"CLI_VERSION": "100000", "FPM_VERSION": "PHP 10.0.0 (fpm-fcgi)"}, "127.0.0.1:9000;", "127.0.0.1:9000;"),
            ("old executable after upgrade", True, {"FPM_EXE_TARGET": "/usr/local/sbin/php-fpm (deleted)"}, "127.0.0.1:9000;", "127.0.0.1:9000;"),
            ("default named upstream", True, {}, "dft_php;", "127.0.0.1:9000;"),
            ("old CLI", False, {"CLI_VERSION": "70333"}, "127.0.0.1:9000;", "127.0.0.1:9000;"),
            ("broken CLI", False, {"CLI_STATUS": "1"}, "127.0.0.1:9000;", "127.0.0.1:9000;"),
            ("unparseable CLI", False, {"CLI_VERSION": "PHP 8.4.23"}, "127.0.0.1:9000;", "127.0.0.1:9000;"),
            ("old running FPM", False, {"FPM_VERSION": "PHP 7.3.33 (fpm-fcgi)"}, "127.0.0.1:9000;", "127.0.0.1:9000;"),
            ("CLI executable is not FPM", False, {"FPM_VERSION": "PHP 8.4.23 (cli)"}, "127.0.0.1:9000;", "127.0.0.1:9000;"),
            ("FPM unavailable", False, {"SS_OUTPUT": ""}, "127.0.0.1:9000;", "127.0.0.1:9000;"),
            ("FPM executable fails", False, {"FPM_STATUS": "1"}, "127.0.0.1:9000;", "127.0.0.1:9000;"),
            ("listener is not PHP-FPM", False, {"FPM_EXE_TARGET": "/usr/bin/custom-server"}, "127.0.0.1:9000;", "127.0.0.1:9000;"),
            ("cannot identify listener", False, {"READLINK_STATUS": "1"}, "127.0.0.1:9000;", "127.0.0.1:9000;"),
            ("custom backend", False, {}, "127.0.0.1:9001;", "127.0.0.1:9000;"),
            ("custom named upstream", False, {}, "dft_php;", "127.0.0.1:9073;"),
            ("mixed named upstream", False, {}, "dft_php;", "127.0.0.1:9000;\nserver 127.0.0.1:9001;"),
        )
        (conf / "503include-only.conf").write_text((ROOT / "config/nginx/503include-only.conf").read_text(encoding="utf-8"), encoding="utf-8")
        php_stock = (ROOT / "config/nginx/php.conf").read_text(encoding="utf-8").replace("/usr/local/nginx/conf/", str(conf) + "/")
        upstream_stock = (ROOT / "config/nginx/default_phpupstream.conf").read_text(encoding="utf-8")
        for label, allowed, overrides, backend, upstream in cases:
            (conf / "php.conf").write_text(php_stock.replace("fastcgi_pass dft_php;", "fastcgi_pass " + backend), encoding="utf-8")
            (conf / "default_phpupstream.conf").write_text(upstream_stock.replace("server 127.0.0.1:9000;", "server " + upstream), encoding="utf-8")
            result = shell(helper + "\nwp_php_preflight\n", **{**defaults, **overrides})
            assert (result.returncode == 0) == allowed, (label, result.stdout, result.stderr)
        variants = (
            ("stock upstream", True, php_stock, upstream_stock, {}),
            ("enabled FastCGI keepalive", True, php_stock.replace("#fastcgi_keep_conn on;", "fastcgi_keep_conn on;"), upstream_stock, {}),
            ("custom timeouts", True, php_stock.replace("360s;", "120s;"), upstream_stock, {}),
            ("inline tuning", True, php_stock.replace("fastcgi_connect_timeout 360s;", "fastcgi_connect_timeout 60s; fastcgi_send_timeout 90s;"), upstream_stock, {}),
            ("custom upstream keepalive", True, php_stock, upstream_stock.replace("keepalive 2;", "keepalive 8;"), {}),
            ("quoted backend", True, php_stock.replace("fastcgi_pass dft_php;", 'fastcgi_pass "dft_php";'), upstream_stock, {}),
            ("directive text in quoted value", True, php_stock.replace("fastcgi_connect_timeout 360s;", 'fastcgi_param EXAMPLE "include bad; fastcgi_pass 127.0.0.1:9073;";'), upstream_stock, {}),
            ("inline old backend after tuning", False, php_stock.replace("fastcgi_connect_timeout 360s;", "fastcgi_connect_timeout 360s; fastcgi_pass 127.0.0.1:9073;"), upstream_stock, {}),
            ("hash in unquoted value", True, php_stock.replace("fastcgi_connect_timeout 360s;", "fastcgi_param EXAMPLE literal#value;"), upstream_stock, {}),
            ("backend hidden after literal hash", False, php_stock + "location /hidden { set $sample literal#value; fastcgi_pass 127.0.0.1:9073;\nreturn 404; }", upstream_stock, {}),
            ("unquoted quote syntax", False, php_stock + 'location /hidden { set $sample literal"; fastcgi_pass 127.0.0.1:9073;"; }', upstream_stock, {}),
            ("variable backend", False, php_stock.replace("fastcgi_pass dft_php;", "fastcgi_pass $backend;"), upstream_stock, {}),
            ("unterminated quote", False, php_stock + 'fastcgi_param EXAMPLE "unterminated', upstream_stock, {}),
            ("unmatched block", False, php_stock + "location /hidden { fastcgi_pass 127.0.0.1:9000;", upstream_stock, {}),
            ("legacy 128k zone", True, php_stock, upstream_stock.replace("256k", "128k"), {}),
            ("comments and whitespace", True, "# local notes\n" + php_stock.replace("    fastcgi_pass dft_php;", "\tfastcgi_pass dft_php;  "), upstream_stock.replace("  server", "\t server"), {}),
            ("GEOIP disabled", True, re.sub(r"(?m)^fastcgi_param GEOIP_", "#fastcgi_param GEOIP_", php_stock), upstream_stock, {}),
            ("SERVER_NAME host variant", True, php_stock.replace("$server_name;", "$http_host;"), upstream_stock, {}),
            ("included additional pool", False, php_stock, upstream_stock.replace("keepalive 2;", "keepalive 2; include /etc/nginx/extra-pool.conf;"), {}),
            ("inline additional pool", False, php_stock, upstream_stock.replace("server 127.0.0.1:9000;", "server 127.0.0.1:9000; server 127.0.0.1:9073;"), {}),
            ("inline additional PHP location", False, php_stock + "location = /legacy.php { fastcgi_pass 127.0.0.1:9073; }\n", upstream_stock, {}),
            ("additional PHP include", False, php_stock + "include /etc/nginx/extra-php.conf;\n", upstream_stock, {}),
            ("inline directive after GEOIP", False, php_stock.replace("$geoip_country_code;", "$geoip_country_code; include /etc/nginx/extra-php.conf;"), upstream_stock, {}),
            ("inactive stock upstream file", False, php_stock, upstream_stock, {"NGINX_DUMP": "# configuration file /etc/nginx/custom.conf:"}),
            ("invalid active configuration", False, php_stock, upstream_stock, {"NGINX_STATUS": "1"}),
        )
        for label, allowed, php_config, upstream, overrides in variants:
            (conf / "php.conf").write_text(php_config, encoding="utf-8")
            (conf / "default_phpupstream.conf").write_text(upstream, encoding="utf-8")
            result = shell(helper + "\nwp_php_preflight\n", **{**defaults, **overrides})
            assert (result.returncode == 0) == allowed, (label, result.stdout, result.stderr)
        (conf / "503include-only.conf").write_text("if ($arg_legacy) { fastcgi_pass 127.0.0.1:9073; }\n", encoding="utf-8")
        result = shell(helper + "\nwp_php_preflight\n", **defaults)
        assert result.returncode == 1 and "503include-only.conf" in result.stdout, (result.stdout, result.stderr)
        print("PASS: PHP preflight accepts verified supported runtimes and rejects unsupported or unknown backends")



def installer_sed(source, text, expected, **variables):
    expressions = re.findall(r'^\s*sed -i ("[^"\n]+"|\'[^\'\n]+\') ', source, re.M)
    assert len(expressions) == expected, expressions
    result = subprocess.run(["bash", "-c", "sed " + " ".join("-e " + item for item in expressions)],
                            input=text, encoding="utf-8", stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                            env={**ENV, "vhostname": "wp-test.invalid", **variables})
    assert result.returncode == 0, result.stderr
    return result.stdout


def installer_cache_vhost(source, vhost, prefix, cache="redis"):
    # Run the actual HTTP-vhost sed expressions without running an installer.
    marker = "if [[ \"$wpscache\" = '" + cache + "' ]]; then\n    if [ -f \"/usr/local/nginx/conf/conf.d/${vhostname}.conf\" ]; then\n"
    branch = source.split(marker, 1)[1].split("    fi\n", 1)[0]
    return installer_sed(branch, vhost, 7 if cache == "fastcgicache" else 6, WPSUBDIR=prefix)


def check_migration():
    source = (ROOT / "inc/cpcheck.inc").read_text(encoding="utf-8")
    assert "wp_config_update || true" in source
    helper = source.split("# Update only recognized WordPress defaults.", 1)[1]
    helper = helper.split("# End WordPress configuration update.", 1)[0]
    helper = "# Update only recognized WordPress defaults." + helper
    assert "python" not in helper.lower(), "Production migration must not require Python"
    standard = (ROOT / GENERATORS[0]).read_text(encoding="utf-8")
    fastcgi = (ROOT / GENERATORS[1]).read_text(encoding="utf-8")
    oauth_block = r"# WordPress OAuth discovery start\n.*?# WordPress OAuth discovery end\n"
    cases = (
        ("stock SSL", "", "ssl-default"), ("live Lets Encrypt SSL", "", "ssl-live"),
        ("live Lets Encrypt subdirectory SSL", "/blog", "ssl-live"),
        ("custom SSL resolver", "", "ssl-custom-resolver"),
        ("inline SSL resolver directive", "", "ssl-inline-resolver"),
        ("direct TCP Super Cache backend", "", "tcp-wpsc"),
        ("direct TCP PHP backends", "", "tcp-all"),
        ("custom direct TCP backend", "", "tcp-custom"),
        ("inline direct TCP directive", "", "tcp-inline"),
        ("FastCGI defaults", "", "fastcgi-default"),
        ("FastCGI mobile caching enabled", "", "fastcgi-mobile"),
        ("FastCGI custom mobile exclusion", "", "fastcgi-custom-mobile"),
        ("FastCGI customized map include", "", "fastcgi-custom-map-include"),
        ("Redis root defaults", "", "redis-default"), ("Redis subdirectory defaults", "/blog", "redis-default"),
        ("root defaults", "", "ok"), ("subdirectory defaults", "/blog", "ok"),
        ("nested defaults", "/sites/news", "ok"), ("custom wpsecure", "", "custom-security"),
        ("site comments preserved", "", "comments"),
        ("unrelated inline include comment", "", "include-comment"),
        ("unrelated quoted include text", "", "include-text"),
        ("legacy upstream zone", "", "upstream-256k"),
        ("custom upstream", "", "custom-upstream"), ("missing upstream", "", "missing-upstream"),
        ("included upstream backend", "", "include-upstream"), ("second upstream backend", "", "second-upstream"),
        ("legacy compatibility tail", "/blog", "compat-tail"),
        ("custom vhost", "", "custom-vhost"), ("conflicting partial discovery", "", "partial"),
        ("inactive sample", "", "inactive"), ("symlink include", "", "symlink"),
        ("quoted shared consumer", "", "quoted-consumer"), ("glob shared consumer", "", "glob-consumer"),
        ("symlink shared consumer", "", "alias-consumer"),
        ("hardlink include", "", "hardlink"), ("changed home URL", "/blog", "home-mismatch"),
        ("custom shared cache map", "", "custom-map"), ("custom Redis config", "", "custom-redis"),
        ("custom FastCGI gates", "", "custom-php"), ("custom PHP include", "", "custom-dependency"),
        ("validation rollback", "", "invalid-new"), ("reload rollback", "", "reload-fails"),
        ("interrupted replacement rollback", "", "interrupted"),
        ("second signal during rollback", "", "double-interrupted"),
        ("dependency changed before apply", "", "dependency-change"),
        ("invalid existing config", "", "invalid-old"), ("stopped nginx", "", "stopped"),
        ("disabled nginx", "", "disabled"),
        ("update lock held", "", "locked"), ("unavailable log directory", "", "log-unavailable"),
        ("no WordPress include directory", "", "no-wordpress"),
    )
    for label, prefix, mode in cases:
        with tempfile.TemporaryDirectory(prefix="cmm-wp-update-") as directory:
            base = Path(directory).resolve()
            conf, domains = base / "nginx", base / "domains"
            for name in ("nginx/wpincludes/wp-test.invalid", "nginx/conf.d", "lock", "tmp", "logs"):
                (base / name).mkdir(parents=True, exist_ok=True)
            local_helper = helper.replace("/usr/local/nginx/conf", str(conf))
            # Source templates still contain the production include prefix.
            local_helper = local_helper.replace(r"s{\Q" + str(conf) + r"\E}{$conf}g",
                                                r"s{\Q/usr/local/nginx/conf\E}{$conf}g")
            for old, new in (("/home/nginx/domains", str(domains)), ("/var/lock", str(base / "lock")),
                             ("/var/tmp", str(base / "tmp")), ("/run/systemd/system", str(base / "systemd"))):
                local_helper = local_helper.replace(old, new)
            files = {}

            def put(relative, text):
                path = conf / relative
                path.parent.mkdir(parents=True, exist_ok=True)
                text = text.replace("/usr/local/nginx/conf", str(conf))
                path.write_text(text, encoding="utf-8")
                path.chmod(0o640)
                files[path] = text
                return path

            secure_name = "wpincludes/wp-test.invalid/wpsecure_wp-test.invalid.conf"
            new_secure = render(heredoc(standard, "wpsecure_", "EEF"), prefix)
            secure = put(secure_name, re.sub(oauth_block, "", new_secure, flags=re.S))
            vhost = render(heredoc(fastcgi if mode.startswith("fastcgi-") else standard, "conf.d/",
                                   "ESS" if mode.startswith("ssl-") else "ENSS"), prefix,
                           DEDI_LISTEN="listen 80;", NGX_LOGFORMAT="main", wpsubdir_value=prefix.lstrip("/"),
                           COMP_HEADER="#spdy_headers_comp 5", LISTENOPT="ssl http2",
                           SUBDIR_INCLUDE=("include " + str(secure) + ";") if prefix else "",
                           NONSUBDIR_INCLUDE="" if prefix else "include " + str(secure) + ";")
            if mode == "redis-default":
                vhost = installer_cache_vhost(standard, vhost, prefix)
            elif mode.startswith("fastcgi-"):
                vhost = installer_cache_vhost(fastcgi, vhost, prefix, "fastcgicache")
                cache_include = render(heredoc(fastcgi, "wpfastcgicache_include_", "FCC"), prefix)
                if mode == "fastcgi-mobile":
                    mobile_sed = "\n".join(line for line in fastcgi.splitlines()
                                            if 'sed -i "s|^if (\\$wpfcgi_cache_mobile' in line)
                    cache_include = installer_sed(mobile_sed, cache_include, 1)
                    assert "#if ($wpfcgi_cache_mobile = 1)" in cache_include
                elif mode == "fastcgi-custom-mobile":
                    cache_include = cache_include.replace("if ($wpfcgi_cache_mobile = 1)", "if ($wpfcgi_cache_mobile = 0)")
                put("wpincludes/wp-test.invalid/wpfastcgicache_include_wp-test.invalid.conf", cache_include)
            elif mode.startswith("ssl-"):
                acme = (ROOT / "addons/acmetool.sh").read_text(encoding="utf-8")
                conversion = acme.split("convert_crtkeyinc() {", 1)[1].split("\n}", 1)[0]
                certs = "\n".join(line for line in vhost.splitlines()
                                  if re.search("ssl_dhparam|ssl_certificate|ssl_trusted_certificate", line)) + "\n"
                vhost = installer_sed(conversion, vhost, 3)
                put("ssl/wp-test.invalid/wp-test.invalid.crt.key.conf", certs)
                put("ssl_include.conf", (ROOT / "config/nginx/ssl_include.conf").read_text(encoding="utf-8"))
                if mode != "ssl-default":
                    live_sed = acme.split("# only enable resolver and ssl_stapling for live ssl certificate deployments", 1)[1].split('        if [ -f ', 1)[0]
                    vhost = installer_sed(live_sed, vhost, 5)
                    assert "\n  resolver " in vhost and "\n  resolver_timeout 10s;" in vhost
                if mode == "ssl-custom-resolver":
                    vhost = vhost.replace("resolver_timeout 10s;", "resolver_timeout 20s;")
                elif mode == "ssl-inline-resolver":
                    vhost = vhost.replace("resolver_timeout 10s;", "resolver_timeout 10s; include /tmp/custom.conf;")
            vhost_path = put("conf.d/wp-test.invalid" + (".ssl.conf" if mode.startswith("ssl-") else ".conf"), vhost)
            for name in ("php.conf", "staticfiles.conf", "drop.conf", "503include-main.conf", "503include-only.conf", "vts_server.conf"):
                put(name, (ROOT / "config/nginx" / name).read_text(encoding="utf-8"))
            put("php-wpsc.conf", (ROOT / "config/nginx/php.conf").read_text(encoding="utf-8"))
            if mode.startswith("tcp-"):
                php_stock = (ROOT / "config/nginx/php.conf").read_text(encoding="utf-8").replace("/usr/local/nginx/conf/", str(conf) + "/")
                # Select the shipped direct TCP alternative, then use the exact
                # WordPress installer command that creates php-wpsc.conf.
                tcp_php = php_stock.replace("    fastcgi_pass dft_php;", "    #fastcgi_pass dft_php;").replace(
                    "    #fastcgi_pass   127.0.0.1:9000;", "    fastcgi_pass   127.0.0.1:9000;")
                host_sed = "\n".join(line for line in standard.splitlines()
                                      if line.startswith('sed -i ') and line.endswith('/usr/local/nginx/conf/php-wpsc.conf'))
                tcp_wpsc = installer_sed(host_sed, tcp_php, 1)
                assert "fastcgi_param  SERVER_NAME        $http_host;" in tcp_wpsc
                if mode == "tcp-custom":
                    tcp_wpsc = tcp_wpsc.replace("127.0.0.1:9000;", "127.0.0.1:9001;")
                elif mode == "tcp-inline":
                    tcp_wpsc = tcp_wpsc.replace("fastcgi_pass   127.0.0.1:9000;", "fastcgi_pass   127.0.0.1:9000; include /tmp/custom.conf;")
                put("php-wpsc.conf", tcp_wpsc)
                if mode == "tcp-all":
                    put("php.conf", tcp_php)
            upstream = "upstream dft_php {\n  zone dftphp_zone 128k;\n  server 127.0.0.1:9000;\n  keepalive 2;\n}\n"
            if mode == "upstream-256k":
                upstream = upstream.replace("128k", "256k")
            elif mode == "custom-upstream":
                upstream = upstream.replace("127.0.0.1:9000", "127.0.0.1:9001")
            elif mode == "include-upstream":
                child = put("additional-php-backend.conf", "server 127.0.0.1:9001;\n")
                upstream = upstream.replace("  keepalive 2;", "  include " + str(child) + ";\n  keepalive 2;")
            elif mode == "second-upstream":
                upstream = upstream.replace("server 127.0.0.1:9000;", "server 127.0.0.1:9000; server 127.0.0.1:9001;")
            if mode != "missing-upstream":
                put("default_phpupstream.conf", upstream)
            if mode == "redis-default":
                put("php-rediscache.conf", render(heredoc(standard, "php-rediscache.conf", "HFF"), prefix))
            put("autoprotect/wp-test.invalid/autoprotect-wp-test.invalid.conf", "")
            put("jetpack_whitelist_ip.conf", "allow 127.0.0.1;\ndeny all;\n")
            put("wpincludes/wp-test.invalid/wpsupercache_wp-test.invalid.conf",
                render(heredoc(standard, "wpsupercache_", "EFF"), prefix))
            cache_map = render(heredoc(fastcgi, "wpfastcgi_cache_map.conf", "FCD"))
            old_map = re.sub(r"^\s+~\S*wp-json\S*\s+1;", "    ~^/wp-json                       1;", cache_map, flags=re.M)
            old_map = re.sub(r"^\s+~\S*well-known/oauth-\S*\s+1;\n", "", old_map, flags=re.M)
            map_path = put("wpfastcgi_cache_map.conf", old_map)
            for name in re.findall(r"^\s*include /usr/local/nginx/conf/([^;]+);", cache_map, re.M):
                put(name, "")
            if mode == "fastcgi-custom-map-include":
                put("wpfastcgi_cache_map_include_mobile.conf", "~CustomMobile 1;\n")
            php = render(heredoc(fastcgi, "php-fastcgicache.conf", "HFF"))
            php_path = put("wpincludes/wp-test.invalid/php-fastcgicache.conf",
                           php.replace(" $http_authorization;", ";"))
            redis = render(heredoc(standard, "rediscache_", "XFF"), prefix)
            redis_path = put("wpincludes/wp-test.invalid/rediscache_wp-test.invalid.conf", re.sub(
                r"# Bypass stored and new OAuth discovery responses[^\n]*\nif .*?\n}\n", "", redis, flags=re.S))
            wp_config = domains / ("wp-test.invalid/public" + prefix) / "wp-config.php"
            wp_config.parent.mkdir(parents=True)
            wp_config.write_text("<?php\ndefine('DB_NAME', 'wordpress');\ndefine('DB_HOST', 'localhost');\n"
                                 "define('DB_USER', 'wpdb123u456');\ndefine('DB_PASSWORD', 'wpdbSecretp789');\n$table_prefix = 'wp_';\n", encoding="utf-8")
            if mode == "custom-security":
                put(secure_name, secure.read_text(encoding="utf-8") + "location = /custom { return 403; }\n")
            elif mode == "comments":
                put(secure_name, "# My site's configuration notes\n" + secure.read_text(encoding="utf-8"))
            elif mode == "include-comment":
                put("conf.d/unrelated.conf", "server { listen 8090; # include examples are in the guide\n return 200; }\n")
            elif mode == "include-text":
                put("conf.d/unrelated.conf", 'server { set $example literal#include; return 200 "include instructions for setup; { quoted braces; }"; }\n')
            elif mode == "compat-tail":
                put(secure_name, secure.read_text(encoding="utf-8") + r"location ~* /(wp-content)/(.*?)\.(zip|gz|tar|bzip2|7z|txt)$ { deny all; }" + "\n")
            elif mode == "custom-vhost":
                put("conf.d/wp-test.invalid.conf", vhost_path.read_text(encoding="utf-8").replace("server {", "server {\n  auth_basic custom;", 1))
            elif mode == "partial":
                put(secure_name, secure.read_text(encoding="utf-8") + "location = /.well-known/oauth-protected-resource { return 403; }\n")
            elif mode == "custom-map":
                put("wpfastcgi_cache_map.conf", map_path.read_text(encoding="utf-8").replace("~^/wp-json", "~^/custom-json"))
            elif mode == "custom-redis":
                put(str(redis_path.relative_to(conf)), redis_path.read_text(encoding="utf-8").replace("set $skip_cache 0;", "set $skip_cache 1;"))
            elif mode == "custom-php":
                put(str(php_path.relative_to(conf)), php_path.read_text(encoding="utf-8").replace("fastcgi_cache_bypass       ", "fastcgi_cache_bypass       $my_policy "))
            elif mode == "custom-dependency":
                put("php-wpsc.conf", (conf / "php-wpsc.conf").read_text(encoding="utf-8").replace("fastcgi_read_timeout 360s;", "fastcgi_read_timeout 600s;"))
            elif mode in ("symlink", "hardlink"):
                target = base / "original-secure"
                secure.rename(target)
                if mode == "symlink":
                    secure.symlink_to(target)
                else:
                    os.link(target, secure)
            elif mode in ("quoted-consumer", "glob-consumer", "alias-consumer"):
                if mode == "quoted-consumer":
                    include = '"' + str(secure) + '"'
                elif mode == "glob-consumer":
                    include = str(secure.parent / "wpsecure_[w]*.conf")
                else:
                    alias = conf / "shared-security.conf"
                    alias.symlink_to(secure)
                    files[alias] = secure.read_text(encoding="utf-8")
                    include = str(alias)
                put("conf.d/custom-second.conf", "server {\n  listen 8081;\n  include " + include
                    + ";\n  location = /private { return 403; }\n}\n")
            originals = {path: path.read_bytes() for path in files}
            original_modes = {path: path.stat().st_mode for path in files}
            sample = put("wpincludes/wp-test.invalid/wpsecure_wp-test.invalid.conf-generated-sample", secure.read_text(encoding="utf-8"))
            files.pop(sample)
            active = [path for path in files if not (mode == "inactive" and path == secure)]
            dump = base / "nginx.dump"

            def write_dump():
                dump.write_text("".join("# configuration file " + str(path) + ":\n" + path.read_text(encoding="utf-8") + "\n" for path in active), encoding="utf-8")

            write_dump()
            executable(base / "bin/nginx", 'echo "nginx diagnostic stderr" >&2; if [[ "$1" = -T ]]; then [[ "$TEST_MODE" != invalid-old ]] || exit 1; cat "$TEST_BASE/nginx.dump"; if [[ "$TEST_MODE" = dependency-change && -f "$TEST_BASE/dump-read" ]]; then echo "# active dependency changed"; fi; touch "$TEST_BASE/dump-read";\nelif [[ "$1" = -t ]]; then echo "nginx validation stdout"; [[ "$TEST_MODE" != invalid-new ]]; else exit 2; fi')
            executable(base / "bin/flock", '[[ "$TEST_MODE" != locked ]]')
            # Production uses GNU mktemp; macOS can provide it via coreutils.
            if shutil.which("gmktemp"):
                (base / "bin/mktemp").symlink_to(shutil.which("gmktemp"))
            executable(base / "bin/pgrep", '[[ "$TEST_MODE" != stopped ]]')
            url = "https://wp-test.invalid" + ("/different" if mode == "home-mismatch" else prefix)
            executable(base / "bin/mariadb", 'if [[ "$*" = *information_schema* ]]; then printf "BASE TABLE\\tInnoDB\\n0\\n"; else printf "home\\t%s\\nsiteurl\\t%s\\n" "$TEST_SITE_URL" "$TEST_SITE_URL"; fi')
            setup = "\n".join((
                "SCRIPT_DIR=" + shlex.quote(str(ROOT)), "CENTMINLOGDIR=" + shlex.quote(str(base / "logs")),
                "SCRIPT_VERSION=wp-test",
                'cmservice_disabled() { [[ "$TEST_MODE" = disabled ]]; }',
                'cmservice_if_enabled() { echo "reload diagnostic"; echo reload >> "$TEST_BASE/reloads"; if [[ "$TEST_MODE" = reload-fails && ! -f "$TEST_BASE/reload-failed" ]]; then touch "$TEST_BASE/reload-failed"; return 1; fi; }',
            ))
            if mode == "log-unavailable":
                (base / "log-blocked").write_text("existing file", encoding="utf-8")
                setup += "\nCENTMINLOGDIR=" + shlex.quote(str(base / "log-blocked"))
            env = dict(PATH=str(base / "bin") + os.pathsep + ENV["PATH"], TEST_MODE=mode,
                       TEST_BASE=str(base), TEST_SITE_URL=url)
            for name in ("python", "python3"):
                executable(base / "bin" / name, 'echo unexpected-python > "$TEST_BASE/python-called"; exit 99')
            if mode in ("interrupted", "double-interrupted"):
                executable(base / "bin/cp", '/bin/cp "$@" || exit $?; for arg; do target=$arg; done; if [[ "$target" = *.wp-update.* ]]; then if [[ -f "$TEST_BASE/one-write" ]]; then kill -TERM "$PPID"; else touch "$TEST_BASE/one-write"; fi; elif [[ "$TEST_MODE" = double-interrupted && "$target" = *.wp-restore.* ]]; then kill -TERM "$PPID"; fi')
            if mode == "no-wordpress":
                (conf / "wpincludes").rename(base / "saved-wpincludes")
            result = shell(setup + "\n" + local_helper + "\nwp_config_update\n", **env)
            if mode == "no-wordpress":
                (base / "saved-wpincludes").rename(conf / "wpincludes")
                assert not list((base / "lock").iterdir()), label
            assert (result.returncode == 0) == (mode not in ("invalid-new", "reload-fails", "interrupted", "double-interrupted", "dependency-change", "log-unavailable")), (label, result.stdout, result.stderr)
            updated = {path for path in originals if path.read_bytes() != originals[path]}
            if mode in ("invalid-new", "reload-fails", "invalid-old", "stopped", "disabled", "interrupted", "double-interrupted", "dependency-change", "locked", "log-unavailable", "no-wordpress"):
                expected = set()
            else:
                expected = {map_path, php_path, redis_path}
                if mode in ("custom-map", "fastcgi-custom-map-include"):
                    expected -= {map_path, php_path}
                if mode == "custom-redis":
                    expected -= {redis_path}
                if mode == "custom-php":
                    expected -= {php_path}
                if mode in ("ok", "redis-default", "comments", "include-comment", "include-text", "compat-tail", "custom-map", "custom-redis", "custom-php", "upstream-256k",
                            "ssl-default", "ssl-live", "tcp-wpsc", "tcp-all", "fastcgi-default", "fastcgi-mobile"):
                    expected.add(secure)
            assert updated == expected, (label, sorted(map(str, updated)), sorted(map(str, expected)), result.stdout, result.stderr)
            assert result.stderr == "", (label, result.stderr)
            assert not (base / "python-called").exists(), label
            if mode in ("stopped", "disabled", "locked", "no-wordpress"):
                assert result.stdout == "", (label, result.stdout)
            elif not updated:
                assert result.stdout.strip(), (label, "Failure or validation notice missing")
            assert all(path.stat().st_mode == original_modes[path] for path in originals), label
            assert sample.read_bytes() == originals[secure], label
            assert not list((base / "tmp").iterdir()), (label, "Temporary files left behind")
            assert not list(conf.rglob("*.wp-update.*")), (label, "Unfinished replacement left behind")
            assert not list(conf.rglob("*.wp-restore.*")), (label, "Unfinished rollback left behind")
            logs = list((base / "logs").glob("centminmod_wp-test_*_wordpress_config_update.log"))
            assert len(logs) == (0 if mode in ("log-unavailable", "no-wordpress") else 1), (label, logs)
            assert set(logs) == set((base / "logs").glob("*.log")), (label, "Logs must match logrotate's *.log pattern")
            assert not list((base / "logs").glob("*.log.*")), (label, "Legacy log suffix")
            if logs:
                log = logs[0].read_text(encoding="utf-8")
                assert logs[0].stat().st_mode & 0o777 == 0o600, label
                assert "Starting WordPress configuration check:" in log and "Finished WordPress configuration check:" in log, (label, log)
                assert f"status={result.returncode}; committed={'y' if updated else 'n'}" in log, (label, log)
                assert all(secret not in log for secret in ("# configuration file ", "DB_PASSWORD", "wpdbSecretp789")), (label, log)
                assert result.stdout.strip() in log, (label, result.stdout, log)
                if mode in ("stopped", "disabled", "locked"):
                    assert "Skipped:" in log, (label, log)
                else:
                    assert "nginx diagnostic stderr" in log, (label, log)
                if mode == "dependency-change":
                    assert "active configuration changed during preparation" in log, log
                if mode in ("invalid-new", "reload-fails", "interrupted", "double-interrupted"):
                    assert "WordPress configuration rollback:" in log, (label, log)
                if updated or mode in ("invalid-new", "reload-fails"):
                    assert "nginx validation stdout" in log, (label, log)
                if updated or mode == "reload-fails":
                    assert "reload diagnostic" in log, (label, log)
            reloads = (base / "reloads").read_text(encoding="utf-8").count("reload") if (base / "reloads").exists() else 0
            assert reloads == (2 if mode == "reload-fails" else 1 if expected else 0), (label, reloads)
            if updated:
                backups = list((base / "logs").glob("wordpress-config-backup.*"))
                assert len(backups) == 1, label
                assert result.stdout == (f"Updated WordPress configuration ({len(updated)} files). "
                                         f"Backups: {backups[0]}\n"), (label, result.stdout)
                for line in (backups[0] / "manifest").read_text(encoding="utf-8").splitlines():
                    filename, number = line.split("\t")
                    assert (backups[0] / (number + ".conf")).read_bytes() == originals[Path(filename)], label
                    assert "Updated: " + filename in log, (label, log)
                before_repeat = {path: path.read_bytes() for path in originals}
                write_dump()
                again = shell(setup + "\n" + local_helper + "\nwp_config_update\n", **env)
                assert again.returncode == 0, (label, again.stdout, again.stderr)
                assert again.stdout == again.stderr == "", (label, again.stdout, again.stderr)
                assert all(path.read_bytes() == before_repeat[path] for path in originals), label
                assert (base / "reloads").read_text(encoding="utf-8").count("reload") == reloads, label
                assert len(list((base / "logs").glob("wordpress-config-backup.*"))) == 1, label
                repeat_logs = set((base / "logs").glob("*_wordpress_config_update.log")) - set(logs)
                assert len(repeat_logs) == 1, (label, repeat_logs)
                repeat_log = repeat_logs.pop().read_text(encoding="utf-8")
                assert "No configuration changes required or eligible." in repeat_log, (label, repeat_log)
                assert "status=0; committed=n" in repeat_log, (label, repeat_log)
                if mode == "custom-security":
                    assert "customized or partial discovery rules" in repeat_log, repeat_log
                    verbose = shell(setup + "\n" + local_helper + "\nWP_CONFIG_UPDATE_VERBOSE=y wp_config_update\n", **env)
                    assert verbose.returncode == 0 and verbose.stderr == "", (verbose.stdout, verbose.stderr)
                    assert "customized or partial discovery rules" in verbose.stdout, verbose.stdout
                    assert all(path.read_bytes() == before_repeat[path] for path in originals), label
                    assert (base / "reloads").read_text(encoding="utf-8").count("reload") == reloads, label
                    assert len(list((base / "logs").glob("wordpress-config-backup.*"))) == 1, label
            if mode == "invalid-new":
                continued = shell(setup + "\n" + local_helper + "\nwp_config_update || true\necho startup-continued\n", **env)
                assert continued.returncode == 0 and continued.stdout.endswith("startup-continued\n"), (continued.stdout, continued.stderr)
                assert all(path.read_bytes() == originals[path] for path in originals), label
                assert not (base / "python-called").exists(), label
    print("PASS: migration defaults, quiet output, private run logs, customizations, backups, rollback and service state")


if __name__ == "__main__":
    check_launcher()
    check_templates()
    check_php_preflight()
    check_migration()
