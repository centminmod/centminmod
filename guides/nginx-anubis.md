# Anubis Bot Challenge for Nginx

Protect a site from scrapers with an [Anubis](https://github.com/TecharoHQ/anubis) proof-of-work challenge, using Centmin Mod 141.00beta01's `addons/anubis.sh`.

## Overview

Anubis makes a browser solve a small proof-of-work puzzle before it can see a page. A browser solves it in the background and gets a cookie. Mass scrapers that don't run JavaScript, or don't want to spend CPU on every page, are stopped. Anubis also has a bot policy file that allows or denies requests by user agent and path.

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
- **Search engines:** crawlers the policy doesn't allow see the challenge page. Test the SEO impact before using it on a public site.
- **HTTPS:** Anubis sets a Secure cookie, so it works on HTTPS vhosts by default. See [Troubleshooting](#the-challenge-repeats-forever-on-a-plain-http-site) for plain HTTP.

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

The default policy already lets through `/.well-known/` (needed for Let's Encrypt HTTP-01 renewals), `robots.txt`, `favicon` and `sitemap.xml`.

### Locations with their own `error_page`

A location with its own `error_page` lines does not inherit the redirect from `anubis_check.conf`. Repeat it in that location:

```nginx
error_page 401 = @redirectToAnubis;
```

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

## Settings

Set these in `/etc/centminmod/custom_config.inc` before running `install`:

| Setting | Default | Purpose |
|---------|---------|---------|
| `ANUBIS_VER` | `1.28.2` | Anubis version. Other versions skip the checksum check |
| `ANUBIS_PORT` | `8923` | Anubis listen port on 127.0.0.1. Set it before the first `install`, which writes it into `centminmod.env` (`BIND`) and `anubis.conf`. To change it later, edit `BIND` in `/etc/anubis/centminmod.env` and the port in `anubis.conf` together, then restart Anubis and reload nginx. Changing `ANUBIS_PORT` and re-running `install` does not move an existing install: it keeps the `BIND` port, uses it for an unedited `anubis.conf`, and prints a warning |
| `ANUBIS_METRICS_PORT` | `8924` | Metrics port on 127.0.0.1. The upstream default, 9090, is also Cockpit's port |

Other Anubis options, such as `DIFFICULTY` or `COOKIE_SECURE`, go in `/etc/anubis/centminmod.env`. Restart the service after changing them.

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

### The challenge repeats forever on a plain HTTP site

The browser drops the Secure cookie over plain HTTP. Use HTTPS, or set `COOKIE_SECURE=false` in `/etc/anubis/centminmod.env` and restart:
```bash
systemctl restart anubis@centminmod
```

### `nginx -t` fails with `unknown directive "auth_request"`

nginx was built without the module. See [Prerequisites](#prerequisites).

### A path still shows the challenge after `auth_request off;`

Check that the request matches that location. `nginx -T` shows the full config. A more specific regex location may be matching the request first.

### The challenge page has no styles or scripts

Another location is serving `/.within.website/` instead of Anubis. `anubis.conf` uses `location ^~ /.within.website/` so that regex locations, such as `drop.conf`'s hidden file rule or `staticfiles.conf`'s `.js`/`.css` rules, don't take over. Make sure the include is at `server {}` level and no other `^~` or exact-match location covers that path.

## Related Guides

- [AI Crawler Maps](nginx-ai-crawlers.md): rate limit or block AI crawlers by category without a challenge
- [Nginx Modules in 141.00beta01](nginx-141-modules.md)
