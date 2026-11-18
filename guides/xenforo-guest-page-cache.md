# XenForo Guest Page Cache on Centmin Mod

Serve XenForo 2.3 pages to guests from a cache, so a flood of guest traffic
doesn't tie up PHP-FPM. There are two ways: nginx's FastCGI cache, or
XenForo's own page cache. This guide sets up either one on Centmin Mod
141.00beta01.

---

## Table of Contents

- [Which cache to use](#which-cache-to-use)
- [Before you start](#before-you-start)
- [Option 1: nginx FastCGI guest cache](#option-1-nginx-fastcgi-guest-cache)
- [Check the nginx cache](#check-the-nginx-cache)
- [Clear or remove the nginx cache](#clear-or-remove-the-nginx-cache)
- [Option 2: XenForo's own page cache](#option-2-xenforos-own-page-cache)
- [The CSRF trade-off with the nginx cache](#the-csrf-trade-off-with-the-nginx-cache)
- [Pitfalls](#pitfalls)

---

## Which cache to use

Measured on one test server: a 4-core / 8-thread Xeon E3-1270 v6, Centmin Mod
141.00beta01, nginx 1.31.8, PHP 8.3, XenForo 2.3.13 with 20,000 users and
100,000 posts, 100 concurrent HTTPS connections over loopback. Your numbers
will differ; the ratios are what matter.

| Guest home page | No page cache | XenForo page cache | nginx FastCGI cache |
|-----------------|---------------|--------------------|---------------------|
| Uncompressed, 139 KB | 178 req/s | 1,232 req/s | 8,896 req/s |
| Gzip, 18 KB | 172 req/s | 934 req/s | 40,790 req/s |
| CSRF token | per visitor | per visitor | **shared by guests** for about a minute |
| Style, language and cookie-consent choices | n/a | cached per choice | sent to PHP (not cached) |
| Purge on new posts | n/a | no (expires) | no (expires) |
| Logged-in members | not cached | not cached | not cached |

- **XenForo's own page cache** runs inside PHP: every hit still starts PHP and
  reads Redis, but each guest gets their own CSRF token, and pages are cached
  per style, language and cookie-consent choice. Choose it when correctness
  matters more than raw speed.
- **nginx's FastCGI cache** never touches PHP on a hit, so it is 7 to 44 times
  faster here and keeps PHP-FPM free during guest floods. Cached guests share
  one CSRF token for about a minute (see
  [the CSRF trade-off](#the-csrf-trade-off-with-the-nginx-cache)).

Both expire pages after a fixed time. Neither purges a page when a new post
arrives.

---

## Before you start

- A XenForo 2.3 site on a Centmin Mod 141.00beta01 HTTPS vhost made by `nv`,
  with XenForo's friendly-URL `try_files` line and the `location ^~` rules for
  `/install/data/`, `/internal_data/`, `/src/`, `/library/` and `/admin.php`.
- Keep XenForo's own gzip on. It is on unless `src/config.php` sets
  `$config['enableGzip'] = false;`. With it off, the nginx cache stores the
  uncompressed page and nginx re-compresses it on every gzip hit: in testing,
  gzip hits fell from 40,790 to 1,644 req/s.
- XenForo's page cache (option 2) needs XenForo's cache turned on with Redis.
  Centmin Mod 141 installs Redis and the phpredis extension by default.
- Busy sites using XenForo's page cache: check
  `sysctl net.ipv4.tcp_tw_reuse`. It should print `2`. With `0`, the
  thousands of short Redis connections per second that cache hits open can fill
  the local port range, and every PHP request slows down (member pages fell
  from 218 to 146 req/s in testing). Current 141.00beta01 code sets `2` on
  EL8, EL9 and EL10 when centmin.sh or centmin-cli.sh runs (unless
  `TCP_TW_REUSE_LOOPBACK='n'` is set in `/etc/centminmod/custom_config.inc`).

Replace `yourdomain.com` and the zone name `xf_yourdomain_com` in every
command and file below.

---

## Option 1: nginx FastCGI guest cache

### Step 1: Cache directory

```bash
mkdir -p /home/fastcgicache_xf/xf_yourdomain_com
chown -R nginx:nginx /home/fastcgicache_xf
```

### Step 2: http-level settings

Create `/usr/local/nginx/conf/xffastcgi_cache_path_includes.conf`. The `map`
blocks are shared by every XenForo site on the server. Add one
`fastcgi_cache_path` line per site.

```nginx
# XenForo guest page cache (http level)
fastcgi_cache_path /home/fastcgicache_xf/xf_yourdomain_com levels=1:2
                   keys_zone=xf_yourdomain_com:32m max_size=1g inactive=10m
                   use_temp_path=off;

# Cookies that make XenForo render a different page go to PHP: session and
# login (xf_session, xf_session_admin, xf_user), a chosen style, style
# variation or language, a cookie-consent choice, and forced database writes.
# Change "xf_" if src/config.php sets another $config['cookie']['prefix'].
map $http_cookie $xf_skip_cookie {
    default 0;
    "~*xf_(session|user|style_id|style_variation|language_id|consent|dbwriteforced)" 1;
}

# Pages with forms or per-visitor content, admin, installer, job runner.
# Matches friendly URLs (/login/) and XenForo's default /index.php?login/ form.
map $request_uri $xf_skip_uri {
    default 0;
    "~*^/(index\.php\?)?(admin\.php|install/|job\.php|login|register|lost-password|logout|account/|conversations/|search/|misc/|attachments/)" 1;
}

# AJAX requests (overlays and the like).
map $http_x_requested_with $xf_skip_xhr {
    default 0;
    "~." 1;
}

# XenForo gzips its HTML whenever Accept-Encoding contains "gzip" (it never
# uses br), so the cache key varies on exactly that.
map $http_accept_encoding $xf_gzip {
    default "";
    "~gzip" gzip;
}

# Store only responses that set the xf_csrf cookie. XenForo sets it only for
# a visitor who has none yet. A page made for a guest who already holds one
# carries that guest's token and no cookie; stored and replayed to other
# guests, its forms would fail XenForo's CSRF check.
map $upstream_cookie_xf_csrf $xf_csrf_unset {
    default 0;
    "" 1;
}
```

Include it from `/usr/local/nginx/conf/nginx.conf`, on the line after
`include /usr/local/nginx/conf/fastcgi_param_https_map.conf;`. That is the
same place Centmin Mod's WordPress cache (menu 22) adds its cache-path include.

```bash
grep -q 'xffastcgi_cache_path_includes.conf' /usr/local/nginx/conf/nginx.conf || \
  sed -i 's|^include /usr/local/nginx/conf/fastcgi_param_https_map.conf;|&\ninclude /usr/local/nginx/conf/xffastcgi_cache_path_includes.conf;|' /usr/local/nginx/conf/nginx.conf
grep -n 'xffastcgi_cache_path_includes' /usr/local/nginx/conf/nginx.conf
```

### Step 3: Site settings

```bash
mkdir -p /usr/local/nginx/conf/xenforo-guest-cache
```

Create `/usr/local/nginx/conf/xenforo-guest-cache/yourdomain.com.conf`:

```nginx
# XenForo guest page cache for yourdomain.com (server level)
fastcgi_cache xf_yourdomain_com;
# XenForo gzips its own HTML when the browser accepts gzip, so the cache key
# includes $xf_gzip ("gzip" or empty, see the map in Step 2).
fastcgi_cache_key "$scheme$request_method$host$request_uri$xf_gzip";
fastcgi_cache_methods GET HEAD;
# 200 responses only, for 60 seconds. 404s and redirects are never stored.
fastcgi_cache_valid 200 60s;
fastcgi_cache_bypass $xf_skip_cookie $xf_skip_uri $xf_skip_xhr;
fastcgi_no_cache $xf_skip_cookie $xf_skip_uri $xf_skip_xhr $xf_csrf_unset
                 $upstream_cookie_xf_session $upstream_cookie_xf_user;
# Guest pages send "Cache-Control: private, no-cache" and set the xf_csrf
# cookie. Store them anyway, but only when they set xf_csrf ($xf_csrf_unset),
# so the stored cookie always matches the token in the stored page. Never
# store a response that sets xf_session or xf_user.
fastcgi_ignore_headers Cache-Control Expires Set-Cookie Vary;
fastcgi_cache_lock on;
fastcgi_cache_lock_timeout 5s;
# Serve the old copy to other guests while one request asks PHP for a new
# one, or while PHP-FPM is down. A returning guest's new page isn't stored
# (see the xf_csrf map), so the next request asks PHP again.
# No fastcgi_cache_background_update: a guest who already has an xf_csrf
# cookie gets a page that isn't stored, so a background refresh made for that
# guest would leave the old page in the cache, served for as long as only such
# guests visit.
fastcgi_cache_use_stale updating error timeout;
add_header X-Page-Cache $upstream_cache_status always;
```

### Step 4: Include it in the vhost

In `/usr/local/nginx/conf/conf.d/yourdomain.com.ssl.conf`, add the include at
server level (outside any `location`), for example on the line before the
server-level `include /usr/local/nginx/conf/php.conf;`. The PHP location in
`php.conf` inherits server-level settings:

```nginx
  include /usr/local/nginx/conf/xenforo-guest-cache/yourdomain.com.conf;
  include /usr/local/nginx/conf/php.conf;
```

The `admin.php` location has its own `php.conf` include and inherits the cache
settings too, but `$xf_skip_uri` sends `/admin.php` past the cache.

### Step 5: Test and reload

```bash
nginx -t && ngxreload
```

---

## Check the nginx cache

Run the same request twice; the second should be a `HIT`:

```bash
D=yourdomain.com
for i in 1 2; do curl -sk -o /dev/null -D - --resolve $D:443:127.0.0.1 https://$D/ | grep -i '^x-page-cache'; done
for i in 1 2; do curl -sk -o /dev/null -D - --resolve $D:443:127.0.0.1 -H 'Accept-Encoding: gzip' https://$D/ | grep -iE '^(x-page-cache|content-encoding)'; done
curl -sk -o /dev/null -D - --resolve $D:443:127.0.0.1 -H 'Cookie: xf_user=1' https://$D/ | grep -i '^x-page-cache'
curl -sk -o /dev/null -D - --resolve $D:443:127.0.0.1 https://$D/login/ | grep -i '^x-page-cache'
```

| Request | Expected `X-Page-Cache` |
|---------|-------------------------|
| Guest `/`, second request | `HIT` |
| Guest `/` with `Accept-Encoding: gzip`, second request | `HIT`, with `content-encoding: gzip` (gzip pages are cached separately, so the first is a `MISS`) |
| Any `xf_session`, `xf_user`, style, language or consent cookie | `BYPASS` |
| `/login/`, `/register/`, `/admin.php`, AJAX requests | `BYPASS` |
| A 404 page | `MISS` every time (not stored) |

---

## Clear or remove the nginx cache

Pages expire after 60 seconds. To clear the cache now:

```bash
rm -rf /home/fastcgicache_xf/xf_yourdomain_com/*
```

To remove the cache from a site, delete its include line from the vhost, then
`nginx -t && ngxreload`. When no site uses it any more, also remove the include
line from `nginx.conf`, the two files and `/home/fastcgicache_xf`.

---

## Option 2: XenForo's own page cache

Add to the end of `src/config.php`, then reload PHP-FPM with `fpmreload`
(Centmin Mod's OPcache only rechecks PHP files every few minutes):

```php
$config['cache']['enabled'] = true;
$config['pageCache']['enabled'] = true;
$config['pageCache']['lifetime'] = 300;
$config['cache']['context']['page']['provider'] = 'Redis';
$config['cache']['context']['page']['config'] = [
    'host' => '127.0.0.1',
    'port' => 6379,
    'database' => 2,
];
```

- `$config['cache']['enabled'] = true;` is required: with XenForo's cache off,
  every cache context, the `page` one included, is off. If `src/config.php`
  already has a Redis cache block, keep it; it sets this line already.
- The page cache needs its own `page` cache context. XenForo does not fall back
  to the main cache for it, so without these lines the page cache silently does
  nothing.
- A separate Redis database (`2` here) keeps cached pages apart from the main
  cache and sessions.
- A cached response has the header `X-XF-Cache-Status: HIT`:

  ```bash
  D=yourdomain.com
  for i in 1 2; do curl -sk -o /dev/null -D - --resolve $D:443:127.0.0.1 https://$D/ | grep -i '^x-xf-cache-status'; done
  ```

- Each cache hit still records the guest's session activity (for "who's
  online"). `$config['pageCache']['recordSessionActivity'] = false;` skips that
  write and raised cache hits from 1,232 to 1,467 req/s in testing.
- XenForo stores the page uncompressed and gzips it in PHP on every hit for
  browsers that accept gzip. That is why its gzip hits are slower than its
  uncompressed ones.
- Turning it on costs logged-in members about 2% (every request sets up the
  page cache and its Redis connection).

---

## The CSRF trade-off with the nginx cache

XenForo protects forms with a CSRF token that must match the visitor's
`xf_csrf` cookie. The nginx cache stores only pages that set a new `xf_csrf`
cookie, and replays that cookie with the page, so the token in a cached page
always matches the cookie it comes with. But every guest who gets that cached
page within about a minute has the same token, and anyone can read it by
fetching the page. That allows login CSRF: tricking a guest's browser into
logging in to an account the attacker controls.

- The settings above send `/login/`, `/register/` and AJAX requests to PHP, so
  visitors who open those pages first get their own token.
- Visitors who already hold the shared cookie from a cached page keep using it.
- A cached page replaces the `xf_csrf` cookie a guest already had. A form the
  guest opened earlier in another tab then fails once with a security error;
  reloading that tab fixes it.
- XenForo's own page cache swaps in a fresh token for every visitor and doesn't
  have this problem. If that matters for your forum, use option 2, or use the
  nginx cache only during guest floods.

---

## Pitfalls

- **`$config['enableGzip'] = false`** with the nginx cache: every gzip hit is
  compressed again by nginx. Keep XenForo's default.
- **Two XenForo sites, one zone:** give each site its own `keys_zone` and
  directory. The `$host` in the cache key keeps their pages apart, but separate
  zones are easier to clear.
- **A different cookie prefix:** change `xf_` in the `map` blocks and in
  `$upstream_cookie_xf_session`, `$upstream_cookie_xf_user` and
  `$upstream_cookie_xf_csrf`.
- **Add-ons that set cookies for guests:** the cache stores any guest page that
  sets `xf_csrf` and not `xf_session` or `xf_user`, and replays every cookie
  that page sets to the guests who get it. XenForo's own page cache skips pages
  that set any other cookie. If an add-on sets a per-visitor cookie for guests,
  add its name to the `$xf_skip_cookie` map (visitors who have it skip the
  cache) and add `$upstream_cookie_<name>` to `fastcgi_no_cache` (pages that
  set it are not stored), or use option 2.
- **Cloudflare in front:** Cloudflare doesn't cache HTML by default and passes
  the cookies through, so the rules above still apply at the origin.
- **Stale guest pages:** new posts show to guests within the lifetime (about a
  minute for nginx, `lifetime` seconds for XenForo). Shorten it if that matters more than
  the hit rate.

---

**Created:** 2026-11-19 · Tested on Centmin Mod 141.00beta01, AlmaLinux 10.3,
nginx 1.31.8, PHP 8.3.36, XenForo 2.3.13
