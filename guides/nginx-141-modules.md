# Nginx Modules in 141.00beta01

What changed for nginx modules in Centmin Mod 141.00beta01, and how to turn on the new optional modules.

## Overview

Centmin Mod 141.00beta01 brings four kinds of nginx module changes:

1. **Security and version updates** for modules Centmin Mod already builds. You get them on your next nginx rebuild.
2. **Two new modules that are on by default:** the nginx JSON module on nginx 1.31.5+, and the ACME client on Angie builds.
3. **New optional modules, all off by default:**
   - QuickJS for njs
   - five Lua libraries
   - JWT authentication
   - a key-value store
   - Redis-backed rate limiting
   - OpenTelemetry tracing
   - HTTP-FLV live streaming
4. **Two deprecated modules:**
   - The third-party sticky module still builds, with a warning.
   - The JA3 TLS fingerprint module prints a warning and is skipped on most builds, including the default nginx 1.31.8.

Nothing optional is built unless you ask for it. A server that keeps its existing settings gets the security updates and the JSON module on its next rebuild. Angie builds also get the ACME client, and ModSecurity users move to the pinned connector (see [Upgrade Notes](#upgrade-notes-for-existing-servers)).

## How to Turn On an Optional Module

All modules are controlled from `/etc/centminmod/custom_config.inc`, followed by an nginx rebuild.

1. **Add the setting** to `/etc/centminmod/custom_config.inc`. Create the file if it does not exist. For example:
   ```bash
   echo "NGINX_AUTHJWT='y'" >> /etc/centminmod/custom_config.inc
   ```
   If the same setting appears more than once, the last line wins.

2. **Rebuild nginx.** Use centmin.sh menu option 4 (Nginx Upgrade / Downgrade) and enter your current nginx version, or `angie` on an Angie server (a version number would switch it to stock nginx). You can also run it without the menu:
   ```bash
   cd /usr/local/src/centminmod
   ./centmin-cli.sh nginx-update 1.31.8   # or: nginx-update angie
   ```

3. **Check the module is in the build.**
   ```bash
   # configure flags of the running nginx
   nginx -V 2>&1 | tr ' ' '\n' | grep -E 'njs|auth-jwt|keyval|rate-limit|otel|http-flv|json'
   # dynamic modules and the lines that load them
   ls -lh /usr/local/nginx/modules/
   cat /usr/local/nginx/conf/dynamic-modules.conf
   nginx -t
   ```

Dynamic modules get their `load_module` line in `/usr/local/nginx/conf/dynamic-modules.conf` automatically, so you don't add it yourself.

**If a module cannot be built:**
- If a download, checksum or dependency install fails, the build stops before `make install`, and the running nginx keeps serving. Two cases are skipped with a warning instead: a Lua library whose download or install fails, and OpenTelemetry when its build dependencies can't be installed. A failed OpenTelemetry download still stops the build.
- If a later step fails before the new nginx is running, menu 4 puts back the previous nginx config and modules. A failure after that leaves the new nginx running, and the build log names the step that failed.
- Some modules are skipped with a warning, and nginx still builds, when they can't work with your build (see each module's notes below). Check the build log under `/root/centminlogs/` for the warning.

**To turn a module off again:**
1. Remove its directives from your nginx config.
2. Set the setting back to `'n'` and rebuild.

For JWT, keyval, rate limit and OpenTelemetry, the rebuild deletes the module's `.so` and `.so.old` files from `/usr/local/nginx/modules/`, unless `NGXDYNAMIC_MANUALOVERRIDE='y'` is set. Lua library files stay in `/usr/local/nginx-dep/lib/lua/5.1` until you delete them.

## Security and Version Updates

These apply on your next nginx rebuild, with no settings to change:

| Module | Version | Notes |
|--------|---------|-------|
| njs | 1.0.1 → 1.0.2 | CVE-2026-101025 (memory exhaustion on malformed XML), CVE-2026-101019 (signed XML validation) |
| ngx_cache_purge | 3.0.3 → 3.0.4 | CVE-2026-108981: a purge could follow symlinks and delete files the worker can access |
| Angie | 1.11.4 → 1.12.4 | 17 CVE fixes. Angie 1.12 reads `/etc/resolv.conf` by default; Centmin Mod's nginx.conf already sets `resolver 127.0.0.1`, so stock configs are unaffected |
| freenginx | 1.30.1 → 1.30.2 | CVE-2026-97629 (TLS session ticket key) |
| headers-more | 0.40 → 0.41 | Content type matches ignore case and parameters |
| echo | 0.65 → 0.66 | |
| ngx_http_redis | 0.4.0-cmm → 0.4.1-cmm | |
| ModSecurity | git master → pinned releases | libmodsecurity `MODSECURITY_LIBVER='3.0.17'` and connector `MODSECURITY_NGINXVER='1.0.4'`, now from github.com/owasp-modsecurity |
| nginx_upstream_check_module | pinned commit | nginx 1.29.6+ gets a patch that fixes a peer lock left held with `hash` and `ip_hash` upstreams |

ModSecurity users should note that connector v1.0.4 runs in nginx's rewrite and preaccess phases. The git master build used before ran in the access phase.

## New Modules On by Default

### nginx JSON module (`NGINX_JSON='y'`)

This module adds `json_set`, which reads a value out of a JSON variable into an nginx variable.

- It is built for stock nginx 1.31.5 and newer. It is not built for Angie or freenginx, which don't have this module.
- It pairs with nginx 1.31.5's `client_body_early_read`, which lets nginx route requests by fields in a JSON request body.
- The MCP proxy templates use it for an optional header check: see [MCP / SSE / LLM Reverse Proxy](nginx-mcp-proxy.md).

```nginx
# http {} level: copy the "method" field of a JSON request body into $rpc_method
json_set $rpc_method $request_body method;
```

### Angie ACME client (`NGINX_ANGIE_ACME='y'`, Angie builds only)

Angie builds get Angie's built-in ACME client (`--with-http_acme_module`). It does nothing until a vhost uses `acme_client`.

- **Port 53 conflict:** DNS-01 challenges, which wildcard certificates need, make Angie answer DNS queries on UDP port 53. That clashes with NSD or any other DNS server on that port. `acme_dns_port` moves the listener.
- HTTP-01 and `acme_hook` setups don't listen on port 53.

See the [Angie ACME documentation](https://en.angie.software/angie/docs/configuration/modules/http/http_acme/).

## Optional Modules

Each section lists the settings to add to `custom_config.inc` before the rebuild.

### QuickJS engine for njs

```bash
NGINX_NJS='y'
NGINX_NJS_QUICKJS='y'
```

This builds QuickJS into njs, so `js_engine qjs;` works in `http`, `server` and `location` blocks. njs stays the default engine wherever you don't set `js_engine`. Since njs 1.0.0, upstream treats its own njs engine as deprecated and recommends `qjs` for new configurations.

- QuickJS (`NGINX_NJS_QUICKJS_VER='2026-06-04'`) is downloaded with a checksum check and compiled into the njs module. A dynamic njs module (`NGXDYNAMIC_NJS='y'`) grows to about 11 MB.
- If njs does not detect QuickJS, the build stops and your running nginx is kept. You never end up with an nginx that rejects `js_engine qjs;`.

Example, with `/usr/local/nginx/conf/njs/hello.js` containing:

```javascript
function hello(r) {
  r.return(200, "hello from QuickJS\n");
}
export default { hello };
```

```nginx
js_import hello from /usr/local/nginx/conf/njs/hello.js;

server {
  ...
  location = /hello {
    js_engine qjs;
    js_content hello.hello;
  }
}
```

### Lua libraries (with `ORESTY_LUANGINX='y'`)

```bash
ORESTY_LUANGINX='y'
ORESTY_LUALIMITTRAFFIC='y'
ORESTY_LUAHTTP='y'
ORESTY_LUAOPENSSL='y'
ORESTY_LUAPROMETHEUS='y'
ORESTY_LUAJA4='y'
```

Turn on only the ones you need. They are pure Lua libraries, installed into `/usr/local/nginx-dep/lib/lua/5.1` on each nginx build, with no change to the nginx binary. If a library fails to download, it is skipped with a warning and any copy installed earlier is kept.

| Setting | Library | Use |
|---------|---------|-----|
| `ORESTY_LUALIMITTRAFFIC` | lua-resty-limit-traffic 0.09 | `resty.limit.req`, `.conn`, `.count`: rate and connection limits per key, per server |
| `ORESTY_LUAHTTP` | lua-resty-http 0.18.0 | HTTP client for Lua code, e.g. fetching JWKS keys or bot lists |
| `ORESTY_LUAOPENSSL` | lua-resty-openssl 1.9.0 | OpenSSL bindings (keys incl. Ed25519, x509, digests). Not for AWS-LC or BoringSSL builds; a static source OpenSSL build also needs `NGINX_COMPILE_EXPORT='y'` |
| `ORESTY_LUAPROMETHEUS` | nginx-lua-prometheus 0.20260912 | Prometheus metrics from nginx |
| `ORESTY_LUAJA4` | lua-resty-ja4 0.2.1 | JA4 (TLS) and JA4H (HTTP) client fingerprints. Replaces the deprecated JA3 module |

**Prometheus metrics example** (http level):

```nginx
lua_shared_dict prometheus_metrics 10M;
init_worker_by_lua_block {
  prometheus = require("prometheus").init("prometheus_metrics")
  metric_requests = prometheus:counter("nginx_http_requests_total",
    "Number of HTTP requests", {"host", "status"})
}
log_by_lua_block {
  metric_requests:inc(1, {ngx.var.server_name, ngx.var.status})
}

server {
  listen 127.0.0.1:9145;
  location /metrics {
    content_by_lua_block { prometheus:collect() }
  }
}
```

**JA4 fingerprint example** (HTTPS server):

```nginx
server {
  listen 443 ssl;
  ...
  ssl_client_hello_by_lua_block { require("resty.ja4").compute() }

  set $ja4 "-";
  set $ja4h "-";
  access_by_lua_block {
    ngx.var.ja4  = require("resty.ja4").get() or "-"
    ngx.var.ja4h = require("resty.ja4h").compute() or "-"
  }
  # add $ja4 and $ja4h to a log_format to record them
}
```

JA4 notes:
- TLS JA4 needs nginx built against OpenSSL 1.1.1 or newer, which is the default system OpenSSL on EL8, EL9 and EL10.
- AWS-LC and BoringSSL builds reject `ssl_client_hello_by_lua*`, but JA4H still works on them.
- JA4H as a method is covered by the FoxIO licence.

### JWT authentication (`NGINX_AUTHJWT='y'`)

This builds [kjdev/nginx-auth-jwt](https://github.com/kjdev/nginx-auth-jwt) 0.15.0. It checks JSON Web Tokens against a key file or JWKS, and exposes claims as `$jwt_claim_*` variables.

- It is a dynamic module unless `NGXDYNAMIC_AUTHJWT='n'`.
- The build installs `jansson-devel` when it is missing.
- It is skipped with a warning when nginx compiles in a static, source-built OpenSSL or LibreSSL.

```nginx
location /api/ {
  auth_jwt "api";
  auth_jwt_key_file /usr/local/nginx/conf/jwks/api.json;
  proxy_set_header X-User $jwt_claim_sub;
  proxy_pass http://127.0.0.1:8080;
}
```

`return` runs before authentication in nginx's phase order. A location that only does `return 200` answers without checking the token, so test with a location that proxies or serves files.

### Key-value store (`NGINX_KEYVAL='y'`, optional `NGINX_KEYVAL_REDIS='y'`)

This builds [kjdev/nginx-keyval](https://github.com/kjdev/nginx-keyval) 0.5.0. It maps a key, such as a cookie or header, to a variable stored in shared memory.

- It works in `http`, and also in `stream` when `NGINX_STREAM='y'`, which is the default.
- It is dynamic unless `NGXDYNAMIC_KEYVAL='n'`.
- It has no REST API or cluster sync, unlike the NGINX Plus module of the same name.

```nginx
keyval_zone zone=flags:1m;
keyval $cookie_beta $beta_flag zone=flags;
```

**`NGINX_KEYVAL_REDIS='y'`** adds `keyval_zone_redis` for zones stored in Redis, and installs `hiredis-devel` from EPEL.

- Each lookup opens a Redis connection and runs blocking commands inside the nginx worker, so keep Redis on localhost.
- `command_timeout` can't interrupt a Redis that accepts the connection but never answers while nginx.conf sets `timer_resolution`, which Centmin Mod sets to 100ms. The worker then waits until Redis closes the connection.

```nginx
keyval_zone_redis zone=sessions hostname=127.0.0.1 port=6379;
```

### Redis rate limiting (`NGINX_RATELIMIT='y'`)

This builds [weserv/rate-limit-nginx-module](https://github.com/weserv/rate-limit-nginx-module), pinned to a commit with a checksum. Limits are shared by every server that uses the same Redis, Valkey or KeyDB, and nginx reaches it through a normal upstream without blocking. The module is dynamic unless `NGXDYNAMIC_RATELIMIT='n'`.

**Runtime requirement:** your Redis must load the [redis-rate-limiter](https://github.com/onsigntv/redis-rate-limiter) module (`loadmodule /path/to/ratelimit.so`). Without it, Redis rejects every rate limit command. Centmin Mod builds the nginx side only.

```nginx
upstream ratelimit_redis {
  server 127.0.0.1:6379;
  keepalive 16;
}

server {
  ...
  location /api/ {
    rate_limit $remote_addr requests=60 period=1m burst=10;
    rate_limit_pass ratelimit_redis;
    rate_limit_headers on;
    proxy_pass http://127.0.0.1:8080;
  }
}
```

If you only need a per-minute counter, the commented example in `/usr/local/src/centminmod/example/custom_config.inc` (`NGINX_RATELIMIT` section) does it with lua-resty-redis and no Redis module.

### OpenTelemetry tracing (`NGINX_OTEL='y'`)

This builds [nginx-otel](https://github.com/nginx/nginx-otel) 0.1.2 as a dynamic module, which sends OTLP/gRPC traces to an OpenTelemetry collector.

- **Build time:** the build fetches and compiles gRPC, protobuf and opentelemetry-cpp. That needs outbound access to github.com and storage.googleapis.com, and adds a long C++ compile to every nginx build. The module is about 90 MB.
- **When it is skipped:** nginx still builds, with a warning, when cmake is too old, the build dependencies can't be installed, or nginx doesn't use the system OpenSSL. That covers AWS-LC, BoringSSL, LibreSSL, quicTLS and source OpenSSL builds.

```nginx
# http {} level
otel_exporter {
  endpoint 127.0.0.1:4317;
}
otel_trace on;

server {
  ...
  location / {
    otel_trace_context propagate;
    proxy_pass http://127.0.0.1:8080;
  }
}
```

### HTTP-FLV live streaming (`NGINX_RTMP_HTTPFLV='y'`)

This builds [nginx-http-flv-module](https://github.com/winshining/nginx-http-flv-module) 1.2.15, a maintained fork of nginx-rtmp-module.

- It accepts the same `rtmp {}` configuration, and adds HTTP-FLV playback.
- It replaces `NGINX_RTMP`, and the two are never built together. If both are set, HTTP-FLV is built and a warning is printed.
- Its upstream tests nginx 1.30.x, so test on nginx 1.31.x before using it in production.

The `rtmp {}` block goes at the top level of `/usr/local/nginx/conf/nginx.conf`, outside `http {}`:

```nginx
rtmp {
  server {
    listen 1935;
    application live {
      live on;
    }
  }
}
```

```nginx
# in a vhost server {} block
location /live {
  flv_live on;
  chunked_transfer_encoding on;
}
```

Publishers need TCP port 1935 open in CSF if they connect from outside the server. Players use URLs such as `http://example.com/live?port=1935&app=live&stream=mystream`.

## Deprecated Modules

Both print a deprecation warning during the nginx build.

### Third-party sticky module (`NGINX_STICKY='y'`)

nginx-sticky-module-ng has had no change since 2016. nginx 1.29.6+ and Angie have a native `sticky` directive. Move your upstreams to it and set `NGINX_STICKY='n'`:

```nginx
upstream backend {
  server 10.0.0.1:8080;
  server 10.0.0.2:8080;
  sticky cookie srv_id expires=1h httponly secure;
}
```

While `NGINX_STICKY='y'`, Centmin Mod disables the native module, because both register `sticky`. The third-party module also now compiles on nginx 1.23.0+, Angie and freenginx, which it didn't before.

### JA3 TLS fingerprint (`NGINX_TLS_FINGERPRINT='y'`)

The JA3 module patches the OpenSSL 1.1.1 source. It is only built when nginx links that patched source-built OpenSSL and its nginx patch applies. Otherwise it is skipped with a message instead of failing the build.

On EL8, EL9 and EL10 the default system OpenSSL means it is skipped. Use JA4 instead (`ORESTY_LUAJA4='y'`, see above).

## Upgrade Notes for Existing Servers

- **A default rebuild adds `--with-http_json_module`** and picks up the updated module versions. It also removes these modules from `/usr/local/nginx/modules/` while their settings are off:
  - `ngx_otel_module.so`
  - `ngx_http_keyval_module.so` and `ngx_stream_keyval_module.so`
  - `ngx_http_auth_jwt_module.so`
  - `ngx_http_rate_limit_module.so`

  `NGXDYNAMIC_MANUALOVERRIDE='y'` keeps them.
- **New config templates are copied only when missing.** On the next centmin.sh run, `/usr/local/nginx/conf/mcp_proxy.conf`, `mcp_proxy_http.conf` and empty `aicrawlers/` maps are copied in, and existing files are never overwritten. Your own `botlimit.conf` is not changed: see [AI Crawler Maps](nginx-ai-crawlers.md) for updating it.
- **ModSecurity moves to the pinned v1.0.4 connector** on the next rebuild (see the phase note above).

## Troubleshooting

### The module is not in `nginx -V` after a rebuild

1. Check the setting is spelled exactly as above and is the last line for that name in `/etc/centminmod/custom_config.inc`.
2. Search the newest nginx build log in `/root/centminlogs/` for a skip warning about the module. JWT, OpenTelemetry and JA3 are skipped on builds they can't support.
3. Make sure the rebuild finished. A failed rebuild leaves the previous nginx running.

### `nginx -t` fails with "unknown directive"

The module providing that directive is not loaded:
- If it is dynamic, check `/usr/local/nginx/conf/dynamic-modules.conf` has its `load_module` line and that nginx.conf includes that file.
- `json_set` fails on Angie, on freenginx, and on nginx older than 1.31.5.

If njs is loaded but was built without QuickJS, `js_engine qjs;` fails with a different error: nginx knows the directive but rejects the `qjs` value. Set `NGINX_NJS_QUICKJS='y'` and rebuild.

### Lua code fails with "module 'resty.openssl...' not found"

Rebuild nginx after setting `ORESTY_LUAOPENSSL='y'`. The rebuild adds `5.1/?/init.lua` to `lua_package_path`, which lua-resty-openssl needs.

### Rate limit requests fail with errors from Redis

Redis must load the redis-rate-limiter module. Check with `redis-cli MODULE LIST`.

## Related Guides

- [AI Crawler Maps](nginx-ai-crawlers.md)
- [Anubis Bot Challenge](nginx-anubis.md)
- [MCP / SSE / LLM Reverse Proxy](nginx-mcp-proxy.md)
