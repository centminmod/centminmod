# MCP / SSE / LLM Reverse Proxy

Put a Model Context Protocol (MCP) server, a Server-Sent Events (SSE) stream, or a streaming LLM API behind Centmin Mod nginx with the `mcp_proxy.conf` templates added in 141.00beta01.

## Overview

MCP servers, SSE endpoints and LLM APIs stream their responses. Events and tokens arrive over seconds or minutes, and the client needs each one as soon as it is sent. Centmin Mod's defaults buffer proxied responses (`proxy.conf`), gzip proxied JSON (`gzip_proxied any` in `nginx.conf`), and time out after 600 seconds without data (`proxy.conf`). That holds events back or cuts long streams off.

Two templates handle this:

| File | Where it goes | What it does |
|------|---------------|--------------|
| `/usr/local/nginx/conf/mcp_proxy_http.conf` | once, at `http {}` level | The Origin allow list, and the optional JSON body check setup |
| `/usr/local/nginx/conf/mcp_proxy.conf` | in the proxied `location {}` | Allows only GET, POST and DELETE. Returns 403 for disallowed origins. Turns off buffering, caching and gzip (brotli and zstd lines are there, commented out), sets 1-hour read and send timeouts, and sets the proxy headers |

No vhost includes them by default.

**Why the Origin check:** the MCP specification requires servers to reject requests whose `Origin` header is present but not allowed, with HTTP 403. This stops DNS rebinding attacks, where a web page tricks a browser into calling a server on your network. Requests without an `Origin` header pass: command line, desktop and server-side MCP clients don't send one.

## Prerequisites

1. **Centmin Mod 141.00beta01.** New installs have both templates in `/usr/local/nginx/conf/`. On older servers, the next centmin.sh run copies them in if they're missing, or copy them yourself:
   ```bash
   cp -n /usr/local/src/centminmod/config/nginx/mcp_proxy.conf /usr/local/src/centminmod/config/nginx/mcp_proxy_http.conf /usr/local/nginx/conf/
   ```
2. **An nginx vhost** for the domain, for example from centmin.sh menu option 2, with HTTPS.
3. **The MCP server or API running** on a local port, for example `127.0.0.1:8000`.

## Step 1: Include the http-level File

Add this once at `http {}` level. You can put it inside `http {}` in `/usr/local/nginx/conf/nginx.conf`, or at the top of a vhost file outside its `server {}` block:

```nginx
include /usr/local/nginx/conf/mcp_proxy_http.conf;
```

Include it only once, even when several vhosts proxy MCP servers, because it defines nginx variables.

## Step 2: Add the Upstream and Location

In the vhost file, for example `/usr/local/nginx/conf/conf.d/example.com.ssl.conf`:

```nginx
# http level: top of the vhost file, outside server {}
upstream mcp_backend {
  server 127.0.0.1:8000;
  keepalive 16;
}

server {
  ...
  location /mcp {
    include /usr/local/nginx/conf/mcp_proxy.conf;
    proxy_pass http://mcp_backend;
  }
}
```

Use `mcp_proxy.conf` instead of `proxy.conf` in this location, not as well as it. Then apply the change:

```bash
nginx -t && ngxreload
```

### Several backends

MCP clients on protocol revisions 2025-03-26 to 2025-11-25 keep a session in the `Mcp-Session-Id` header. Send each session to the same backend:

```nginx
upstream mcp_backend {
  server 127.0.0.1:8000;
  server 127.0.0.1:8001;
  hash $http_mcp_session_id consistent;
}
```

For LLM inference backends that respond at different speeds, nginx 1.31.0+ can pick the backend with the lowest average time to the response header:

```nginx
upstream llm_backend {
  server 10.0.0.11:8080;
  server 10.0.0.12:8080;
  least_time header;
}
```

## Step 3: Allow Your Web App Origins

Browser-based MCP clients send an `Origin` header, and only listed origins pass. Edit the map in `/usr/local/nginx/conf/mcp_proxy_http.conf` and add each web app origin exactly as `scheme://host[:port]`:

```nginx
map $http_origin $mcp_origin_allowed {
  default                        0;
  ""                             1;
  "https://app.example.com"      1;
}
```

Don't allow an origin just because it matches the request's `Host`. A DNS rebinding page sends a matching `Host` header too.

Browser clients on other origins also send `OPTIONS` preflight requests, which the template answers with 405. If you need them, copy the template under a new name, add `OPTIONS` to the method check, and answer preflight requests with the CORS headers your app needs.

## Step 4: Test

```bash
# disallowed method: 405
curl -s -o /dev/null -w '%{http_code}\n' -X PUT https://example.com/mcp

# Origin present and not allowed: 403
curl -s -o /dev/null -w '%{http_code}\n' -X POST -H 'Origin: https://evil.example' https://example.com/mcp

# streaming: events print as the server sends them, not all at the end
curl -N -H 'Accept: text/event-stream' https://example.com/mcp
```

Then connect your MCP client to `https://example.com/mcp`.

## Changing a Setting

`mcp_proxy.conf` sets these single-value directives: `proxy_buffering`, `proxy_cache`, `proxy_read_timeout`, `proxy_send_timeout`, `proxy_http_version` and `gzip`. If the same location sets one of them again, `nginx -t` reports it as a duplicate. Extra `proxy_set_header` lines in the location are fine, and add to the template's headers.

To change one of those values, copy the template under a new name and edit the copy. For example, to use `proxy_http_version 2;` for an HTTP/2 backend (nginx 1.29.4+):

```bash
cp /usr/local/nginx/conf/mcp_proxy.conf /usr/local/nginx/conf/mcp_proxy_h2.conf
```

**Brotli or zstd:** if you built nginx with brotli or zstd (`NGINX_LIBBROTLI='y'`, `NGINX_ZSTD='y'`, both off by default) and turned them on, uncomment `brotli off;` or `zstd off;` in the template. Compression holds SSE events back. Without the module, nginx rejects those directives.

## Optional: Check Mcp-Method and Mcp-Name Headers

Clients on MCP protocol revision 2026-07-28 copy the JSON-RPC method, and the tool, prompt or resource name, into `Mcp-Method` and `Mcp-Name` request headers, so gateways can route without reading the body. If your nginx routes or rate limits on those headers, it should check they match the body. The specification says intermediaries answer 400 when they don't.

**Requirements:** nginx 1.31.5+ built with `NGINX_JSON='y'` (the default). Check with:

```bash
nginx -V 2>&1 | grep -o with-http_json_module
```

Angie and freenginx don't have `json_set`.

**To turn it on:**

1. In `/usr/local/nginx/conf/mcp_proxy_http.conf`, uncomment the block from `#map "$request_method:$uri" $mcp_body_early_read {` to the end of the file. The map matches the exact method and URI, so change `/mcp` in `"POST:/mcp"` to your MCP location's path and keep the `POST:` prefix.

2. In `/usr/local/nginx/conf/mcp_proxy.conf`, uncomment the two `if` blocks under `# Optional header check`.

3. In the vhost's `server {}` block, make nginx read the MCP request body before the location runs, and keep it in memory:
   ```nginx
   client_body_early_read $mcp_body_early_read;
   client_body_buffer_size 1m;
   ```

4. Apply the change:
   ```bash
   nginx -t && ngxreload
   ```

Test it:

```bash
# header says tools/list, body says tools/call: 400
curl -s -o /dev/null -w '%{http_code}\n' -X POST https://example.com/mcp \
  -H 'Content-Type: application/json' -H 'Mcp-Method: tools/list' \
  -d '{"jsonrpc":"2.0","id":1,"method":"tools/call","params":{"name":"search"}}'
```

Notes:
- **Not checked:** requests without `Mcp-Method` or `Mcp-Name` headers (clients on older revisions), and Base64-encoded `Mcp-Name` values (`=?base64?...?=`). Those are left to the MCP server.
- **Large bodies:** a body larger than `client_body_buffer_size` goes to a temporary file, so `$request_body` is empty. Requests that carry `Mcp-Method` or `Mcp-Name` headers then get 400. Raise the buffer if your requests are larger.
- **Scope:** `client_body_early_read` must not cover gRPC or WebDAV locations. The `$mcp_body_early_read` map limits it to POST requests to your MCP path.

## Troubleshooting

### Events arrive all at once at the end

Something is still buffering or compressing:
1. Make sure the location includes `mcp_proxy.conf` and not `proxy.conf`.
2. If brotli or zstd is turned on at `http {}` or `server {}` level, uncomment `brotli off;` or `zstd off;` in the template or your copy. gzip is already off in the template.
3. Check whether a CDN or proxy in front of nginx buffers responses.

### Long streams stop after a while

`mcp_proxy.conf` allows 1 hour between reads. For longer idle periods, copy the template and raise `proxy_read_timeout`. A CDN or proxy in front of nginx may have a shorter timeout of its own.

### Browser client gets 403

Its origin is not in the `$mcp_origin_allowed` map. Add it as in [Step 3](#step-3-allow-your-web-app-origins), then `nginx -t && ngxreload`.

### Requests fail with 400 after turning on the header check

The headers don't match the body, or the body went to a temporary file (see the notes above). The vhost's access log shows the 400 responses. Send the same request without the `Mcp-*` headers: if that passes, the headers and body disagree.

### `nginx -t` fails with `"..." directive is duplicate`

The location sets a single-value directive that `mcp_proxy.conf` already sets. Remove it from the location, or use a copy of the template, as in [Changing a Setting](#changing-a-setting).

## Related Guides

- [Nginx Docker Reverse Proxy](nginx-docker-reverse-proxy.md): general reverse proxy setup
- [Nginx Modules in 141.00beta01](nginx-141-modules.md): the JSON module and other new modules
