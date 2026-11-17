# Anubis Bot Challenge for Nginx

Protect a site from scrapers with an [Anubis](https://github.com/TecharoHQ/anubis) proof-of-work challenge, using Centmin Mod 141.00beta01's `addons/anubis.sh`.

## Overview

Anubis checks each request against a bot policy. A client the policy finds suspicious must solve a small proof-of-work puzzle in its browser before it sees the page. With the default policy that `addons/anubis.sh` installs:

- Requests whose user agent claims to be a browser, because it contains `Mozilla` or `Opera`, get the challenge. A browser solves it in the background and gets a cookie that is valid for 7 days.
- Known AI crawlers, headless browsers and requests without a user agent get 403, even for `robots.txt`.
- Major search engine crawlers are allowed when both their user agent and their IP address match the crawler's published ranges.
- Other clients, such as `curl`, feed readers and API clients that don't claim to be a browser, pass without a challenge unless a policy rule matches them.
- A network that keeps visiting Anubis's honeypot pages gets a much harder challenge.

So Anubis mainly stops scrapers that pretend to be browsers. To challenge or deny more clients, add rules to the policy file. Sites behind Cloudflare score differently, see [Sites behind Cloudflare](#sites-behind-cloudflare).

Centmin Mod runs Anubis in **subrequest mode**: nginx keeps serving your site, and asks Anubis about each request through the `auth_request` module.

```
Browser → nginx ──auth_request──→ Anubis (127.0.0.1:8923)
              │                     ├─ 200: valid cookie, allowed → nginx serves the page
              │                     ├─ 401: no valid cookie → redirect to the challenge
              │                     └─ 403: denied by the bot policy
              └─ serves your site as usual
```

The addon is optional and never runs by itself. It installs the upstream Anubis RPM (x86_64, version 1.28.2 by default, checksum checked) and runs one service, `anubis@centminmod`, listening on 127.0.0.1 only.

**Things to know before you start:**
- **JavaScript:** browsers need it turned on to pass.
- **Search engines:** a crawler that claims to be a browser and isn't on the policy's allow list sees the challenge page. Test the SEO impact before using it on a public site.
- **HTTPS:** Anubis sets a Secure cookie, so it works on HTTPS vhosts by default. See [Troubleshooting](#the-challenge-repeats-forever-on-a-plain-http-site) for plain HTTP.
- **Cloudflare:** a site behind Cloudflare needs two extra changes, see [Sites behind Cloudflare](#sites-behind-cloudflare).

## Prerequisites

1. **Centmin Mod 141.00beta01**, with `/usr/local/src/centminmod` updated (`cmupdate`).

2. **nginx built with the auth_request module.** It is off by default. Check:
   ```bash
   nginx -V 2>&1 | grep -o with-http_auth_request_module
   ```
   If nothing is printed, turn it on and rebuild nginx:
   ```bash
   echo "NGINX_AUTHREQ='y'" >> /etc/centminmod/custom_config.inc
   cd /usr/local/src/centminmod
   ./centmin.sh   # menu option 4, enter the same nginx version (or angie on an Angie server)
   ```

3. **An HTTPS vhost** to protect, for example one created with centmin.sh menu option 2 and a Let's Encrypt certificate.

## Step 1: Install Anubis

```bash
/usr/local/src/centminmod/addons/anubis.sh install
```

This installs or upgrades the RPM, writes the config files below, enables and starts `anubis@centminmod`, and checks that nginx has auth_request. The log is saved in `/root/centminlogs/`.

| File | Purpose |
|------|---------|
| `/etc/anubis/centminmod.env` | Service settings: listen addresses, subrequest mode, policy file, and a persistent signing key (mode 0600). Because the key persists, passed challenges survive restarts and upgrades |
| `/etc/anubis/centminmod.botPolicies.yaml` | The upstream default bot policy, with the deny status changed to 403 |
| `/usr/local/nginx/conf/anubis.conf` | nginx helper locations: the challenge pages and the redirect |
| `/usr/local/nginx/conf/anubis_check.conf` | The `auth_request` check lines |

Existing files are left unchanged, so your edits survive a reinstall or upgrade, with two exceptions:
- A policy file whose `status_codes` `DENY` is a 2xx code, or missing, gets `DENY: 403`. The previous file is kept as `centminmod.botPolicies.yaml.bak-<date>`.
- An `anubis.conf` still identical to the shipped copy is rewritten with Anubis's port when that is not the default 8923.

The deny status is changed because nginx's `auth_request` treats any 2xx answer as a pass. The upstream default policy answers denied requests with 200, which nginx would let through.

## Step 2: Protect a Vhost

Add two lines to the vhost's `server {}` block, for example in `/usr/local/nginx/conf/conf.d/example.com.ssl.conf`:

```nginx
server {
  ...
  # once per server {}: Anubis challenge pages and the 401 redirect
  include /usr/local/nginx/conf/anubis.conf;
  # every location of this server
  include /usr/local/nginx/conf/anubis_check.conf;
  ...
}
```

To protect only some paths, put the `anubis_check.conf` include in those `location {}` blocks instead. Keep `anubis.conf` at `server {}` level. Then apply the change:

```bash
nginx -t && nprestart
```

### Paths that must skip the challenge

Feeds, APIs, webhooks and health checks are called by programs that can't solve the challenge. Add `auth_request off;` to their locations:

```nginx
location /api/ {
  auth_request off;
  ...
}
```

The default policy already lets through `/.well-known/` (needed for Let's Encrypt HTTP-01 renewals), `robots.txt`, `favicon` and `sitemap.xml`. Clients the policy denies outright, such as known AI crawlers, still get 403 on these paths.

### Locations with their own `error_page`

A location with its own `error_page` lines does not inherit the redirect from `anubis_check.conf`. Repeat it in that location:

```nginx
error_page 401 = @redirectToAnubis;
```

### Sites behind Cloudflare

Cloudflare changes two things Anubis relies on.

**The visitor IP.** Anubis identifies visitors by the `X-Real-IP` header, which `anubis.conf` sets from nginx's `$remote_addr`. Behind Cloudflare that is a Cloudflare address until nginx restores the visitor's IP. Load Cloudflare's IP ranges, and add the same command to cron as the comment in `/usr/local/nginx/conf/nginx.conf` describes:

```bash
/usr/local/src/centminmod/tools/csfcf.sh auto
```

Then uncomment this line in `/usr/local/nginx/conf/nginx.conf`:

```nginx
include /usr/local/nginx/conf/cloudflare.conf;
```

Cloudflare's IPv6 ranges are only enabled when the server has IPv6 networking. Without this step, Anubis's rules and logs see Cloudflare addresses. The Anubis cookie is also tied to the IP it was issued to, so it stops matching when a request arrives through a different Cloudflare address, and the visitor gets the challenge again.

**Cloudflare's request headers.** The default policy adds weight to any request carrying a `Cdn-Loop`, `Cf-Ray` or `Cf-Visitor` header, and Cloudflare adds those to every request it proxies. Then `curl`, feed readers and API clients get a challenge, and browsers get a harder one. Stop nginx passing these headers to Anubis with three lines inside `location ^~ /.within.website/` in `/usr/local/nginx/conf/anubis.conf`:

```nginx
  proxy_set_header Cdn-Loop "";
  proxy_set_header Cf-Ray "";
  proxy_set_header Cf-Visitor "";
```

nginx doesn't send a header whose value is empty. Requests from Cloudflare Workers still carry `CF-Worker`, which the policy weighs separately.

Apply both changes:

```bash
nginx -t && nprestart
```

### How many checks a page costs

nginx runs the `auth_request` check again after every internal redirect. With `anubis_check.conf` at `server {}` level, a request for `/`, or for a pretty URL that `try_files` sends to `/index.php`, is checked twice, once in `location /` and again in the PHP location. Each static file, such as an image, CSS or JavaScript file, is checked once.

Don't add `auth_request off;` to `location /` to save the second check. `location /` would then serve files without a check, and on WordPress vhosts with WP Super Cache or Cache Enabler that includes every cached page.

## Step 3: Test

```bash
# a browser user agent without the cookie gets a 307 redirect to the challenge
curl -sI -A 'Mozilla/5.0 (X11; Linux x86_64; rv:140.0) Gecko/20100101 Firefox/140.0' https://example.com/ | grep -iE '^(HTTP|location)'
```

Expect status `307` and a `location: https://example.com/.within.website/?redir=https://example.com/` header. Then open the site in a browser: you should see the challenge briefly, then your page. User agents the policy denies get `403`.

Check everything at once:

```bash
/usr/local/src/centminmod/addons/anubis.sh status
```

`status` shows:
- the package and the service
- that the check endpoint answers
- whether nginx has auth_request
- which nginx config files include the Anubis files

## Changing the Policy

Edit `/etc/anubis/centminmod.botPolicies.yaml`, then restart the service:

```bash
systemctl restart anubis@centminmod
```

Keep `DENY: 403` under `status_codes`. A deny status of 200 would let denied requests through nginx, so `install` changes a 2xx deny status back to 403. See the policy section of the [Anubis documentation](https://anubis.techaro.lol/docs/) for the rule format.

Rules can match a request's path, user agent and headers. They can't match its method or query string, because Anubis only sees nginx's check request.

## Settings

Set these in `/etc/centminmod/custom_config.inc` before running `install`:

| Setting | Default | Purpose |
|---------|---------|---------|
| `ANUBIS_VER` | `1.28.2` | Anubis version. Other versions skip the checksum check |
| `ANUBIS_PORT` | `8923` | Anubis listen port on 127.0.0.1. Set it before the first `install`, which writes it into `centminmod.env` (`BIND`) and `anubis.conf`. To change it later, edit `BIND` in `/etc/anubis/centminmod.env` and the port in `anubis.conf` together, then restart Anubis and reload nginx. Changing `ANUBIS_PORT` and re-running `install` does not move an existing install: it keeps the `BIND` port, uses it for an unedited `anubis.conf`, and prints a warning |
| `ANUBIS_METRICS_PORT` | `8924` | Metrics port on 127.0.0.1. The upstream default, 9090, is also Cockpit's port |

Other Anubis options, such as `COOKIE_SECURE`, go in `/etc/anubis/centminmod.env`. Restart the service after changing them.

`DIFFICULTY` in that file has no effect with the default policy. The policy sets the difficulty in its `thresholds` section, at 1, 2, 4 or 6 depending on how suspicious a request looks, and a normal browser gets 2. Change it there.

Don't set `PUBLIC_URL`. It is meant for proxies such as Traefik's forwardAuth. With it set, Anubis answers nginx's check with a redirect, and nginx turns that into an HTTP 500 for every protected page.

### Restrict post-challenge redirects (optional)

After a challenge, Anubis sends the browser back to the page it asked for. By default it accepts any domain there. List the hostnames your protected vhosts use in `/etc/anubis/centminmod.env`, then restart Anubis:

```
REDIRECT_DOMAINS=example.com,www.example.com
```

Wildcards such as `*.example.com` work, up to 4 of them. Add the port for a vhost on a non-standard HTTPS port, for example `example.com:8443`. One Anubis service serves all vhosts, so list every protected hostname. A missing one shows "Redirect domain not allowed" after the visitor passes the challenge.

### Go runtime memory (optional)

Anubis is a Go program, and Go's garbage collector trades CPU time for memory. The defaults suit most servers. On a busy server with spare RAM you can let Anubis use more memory for less garbage collection work. Add to `/etc/anubis/centminmod.env`, then restart Anubis:

```
GOGC=200
GOMEMLIMIT=512MiB
```

`GOGC=200` roughly halves garbage collection CPU time and doubles the heap's extra memory compared with the default of 100, as long as the memory limit isn't reached. `GOMEMLIMIT` is a soft cap: near it, the collector works harder to stay below it. These values are examples. Check Anubis's memory first on the Memory line of `systemctl status anubis@centminmod`, and set the limit well below the RAM you can spare. To compare before and after:

```bash
curl -s http://127.0.0.1:8924/metrics | grep -E '^(go_gc_duration_seconds_count|process_resident_memory_bytes)'
```

Don't set `GOGC=off`. The heap then grows until it reaches `GOMEMLIMIT` and stays near it, and the Go documentation advises against it when other programs share the server's memory, as nginx, PHP-FPM, MariaDB and Redis do here. Don't set `GOMAXPROCS` either. Go already derives it from the CPU count and from a systemd `CPUQuota=` limit.

## Benchmarking

Load testers such as wrk send no user agent by default. The default policy denies those requests with 403, and wrk still counts them in Requests/sec, listing them only as "Non-2xx or 3xx responses". Set a user agent, for example `wrk -H 'User-Agent: bench/1.0' ...`, and check that line.

A non-browser user agent measures Anubis's allow path. Real browsers carry the Anubis cookie, which adds a signature check to every request. Behind Cloudflare, make the header change in [Sites behind Cloudflare](#sites-behind-cloudflare) first, or every request gets the challenge.

## Uninstall

While Anubis is stopped, every protected request fails with HTTP 500. So `uninstall` refuses to run while any nginx config file still uses the Anubis check, and it lists those files.

1. Remove the `include /usr/local/nginx/conf/anubis*.conf;` lines from your vhosts.
2. Apply the change:
   ```bash
   nginx -t && nprestart
   ```
3. Uninstall:
   ```bash
   /usr/local/src/centminmod/addons/anubis.sh uninstall
   ```

This stops the service and removes the RPM. It keeps `/etc/anubis/centminmod.env` (with the signing key), the policy file and the two nginx files for a later reinstall. Delete them by hand if you don't need them.

## Troubleshooting

### Every protected request returns 500

Anubis is not running:
```bash
systemctl status anubis@centminmod
journalctl -u anubis@centminmod -n 50 --no-pager
```

If it is running, check that `PUBLIC_URL` is not set in `/etc/anubis/centminmod.env`.

### The challenge repeats forever on a plain HTTP site

The browser drops the Secure cookie over plain HTTP. Use HTTPS, or set both of these in `/etc/anubis/centminmod.env`:
```
COOKIE_SECURE=false
COOKIE_PARTITIONED=false
```
Browsers only accept a Partitioned cookie when it is also Secure, and Anubis sets Partitioned by default. Then restart:
```bash
systemctl restart anubis@centminmod
```
Changing cookie settings renames the Anubis cookie, so every visitor passes the challenge once more.

### Feeds or APIs get the challenge behind Cloudflare

Cloudflare's request headers raise every request's score. See [Sites behind Cloudflare](#sites-behind-cloudflare).

### Visitors see "Redirect domain not allowed" after the challenge

`REDIRECT_DOMAINS` is set and doesn't list that hostname. Add it, with the port if the vhost uses a non-standard one, and restart Anubis.

### `nginx -t` fails with `unknown directive "auth_request"`

nginx was built without the module. See [Prerequisites](#prerequisites).

### A path still shows the challenge after `auth_request off;`

Check that the request matches that location. `nginx -T` shows the full config. A more specific regex location may be matching the request first.

### The challenge page has no styles or scripts

Another location is serving `/.within.website/` instead of Anubis. `anubis.conf` uses `location ^~ /.within.website/` so that regex locations, such as `drop.conf`'s hidden file rule or `staticfiles.conf`'s `.js`/`.css` rules, don't take over. Make sure the include is at `server {}` level and no other `^~` or exact-match location covers that path.

## Related Guides

- [AI Crawler Maps](nginx-ai-crawlers.md): rate limit or block AI crawlers by category without a challenge
- [Nginx Modules in 141.00beta01](nginx-141-modules.md)
