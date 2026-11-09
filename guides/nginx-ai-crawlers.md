# AI Crawler Maps for Nginx

Sort AI crawlers by what they do. Rate limit or block each kind separately, and catch fake Googlebot and bingbot traffic, with `tools/nginx-aicrawlers.sh` and `botlimit.conf` in Centmin Mod 141.00beta01.

## Overview

AI crawlers do different jobs. Some collect pages to train models. Some index pages for AI search engines that link back to you. Others fetch a page because a person asked an AI assistant to read it. Blocking all of them is rarely what a site wants.

`nginx-aicrawlers.sh` builds two nginx include files from public lists:

| File | Variables it sets |
|------|-------------------|
| `aicrawlers_ua.conf` | `$aicrawler_category`: `training`, `search`, `user` or `other`, from the user agent<br>`$aicrawler_claim`: `google`, `bing`, `openai`, `anthropic`, `perplexity` or `apple` when the user agent claims to be that vendor's crawler |
| `aicrawlers_ip.conf` | `$aicrawler_ipvendor`: the vendor whose published crawler IP list holds the client address, `ipv6` for any other IPv6 address, empty for any other IPv4 address. With no vendor list loaded, it is empty for every client<br>`$aicrawler_verified`: `1` = claimed crawler from its vendor's IPs, `0` = claimed crawler from any other IP (spoofed), empty = no claim, that vendor's list is unavailable, or an IPv6 client of a vendor whose list is IPv4 only |

The sources are:
- the [ai.robots.txt](https://github.com/ai-robots-txt/ai.robots.txt) project's `robots.json`
- the crawler IP lists published by Google, Bing, OpenAI, Anthropic, Perplexity and Apple

These files only set variables and never block anything themselves. `botlimit.conf` turns the variables into its existing levels:

| Level | Effect |
|-------|--------|
| 0 | not rate limited |
| 1 | whitelisted (Googlebot, bingbot and similar) |
| 2 | rate limited by the `botb` zones |
| 3 | blocked by the vhost's `if ($bot_agent = '3')` lines |

The defaults change nothing:

- Every AI category starts at level 0.
- Centmin Mod ships empty map files, so nothing changes until the tool has run.
- Once it has run and loaded Google's and Bing's published IP lists, Googlebot and bingbot user agents from IPs outside those lists drop from whitelist level 1 to rate-limited level 2. They are slowed, never blocked.

## Prerequisites

1. **Centmin Mod 141.00beta01**, with `/usr/local/src/centminmod` updated (`cmupdate`).

2. **jq installed:**
   ```bash
   dnf -y install jq
   ```

3. **botlimit.conf active.** It works only when nginx.conf includes it inside `http {}` and your vhosts use `$bot_agent`.

   Check whether it is included:
   ```bash
   nginx -T 2>/dev/null | grep -c '^# configuration file .*/botlimit\.conf:'
   ```
   `nginx -T` prints that header only for files nginx actually loads, so a commented-out include doesn't count. If it prints `0`, add this line inside the `http {}` block of `/usr/local/nginx/conf/nginx.conf`:
   ```nginx
   include /usr/local/nginx/conf/botlimit.conf;
   ```

4. **A current botlimit.conf.** On servers installed before 141.00beta01, `/usr/local/nginx/conf/botlimit.conf` predates the AI crawler maps. Centmin Mod never overwrites it. See [Step 1](#step-1-update-an-older-botlimitconf).

5. **Real visitor IPs behind Cloudflare or another proxy.** Set up `real_ip` first, for example Centmin Mod's `cloudflare.conf`. Otherwise `$remote_addr` is the proxy's IP, and every claimed crawler fails the IP check.

## Step 1: Update an Older botlimit.conf

Skip this step on new 141.00beta01 installs. Check whether yours already includes the AI crawler maps:

```bash
grep -c aicrawlers /usr/local/nginx/conf/botlimit.conf
```

If the count is `0`:

```bash
# the empty map files the new botlimit.conf includes
mkdir -p /usr/local/nginx/conf/aicrawlers
cp -n /usr/local/src/centminmod/config/nginx/aicrawlers/*.conf /usr/local/nginx/conf/aicrawlers/

# keep a copy of your current file, then install the new one
cp -a /usr/local/nginx/conf/botlimit.conf /usr/local/nginx/conf/botlimit.conf.pre141
cp -f /usr/local/src/centminmod/config/nginx/botlimit.conf /usr/local/nginx/conf/botlimit.conf
nginx -t && ngxreload
```

If `nginx -t` fails, put your old file back before fixing the error, so the next reload doesn't pick up the broken config:

```bash
cp -f /usr/local/nginx/conf/botlimit.conf.pre141 /usr/local/nginx/conf/botlimit.conf
```

If you had edited the user agent list in your old file, copy those edits back. The list is now the `map $http_user_agent $bot_agent_ua` block. `$bot_agent` is still the variable your vhosts use; it is now a final map that combines the user agent list with the AI crawler levels.

## Step 2: Preview with a Dry Run

```bash
/usr/local/src/centminmod/tools/nginx-aicrawlers.sh dry-run
```

This downloads and checks every list, then tests the generated files with `nginx -t`. It writes nothing. Look for these lines in the output:

- `nginx -t on the generated files: ok`
- `dry-run: nothing written`

To see the generated maps themselves:

```bash
/usr/local/src/centminmod/tools/nginx-aicrawlers.sh print
```

## Step 3: Install the Maps

```bash
/usr/local/src/centminmod/tools/nginx-aicrawlers.sh update
```

`update` downloads and validates the lists, writes the files to `/usr/local/nginx/conf/aicrawlers/`, runs `nginx -t`, and reloads nginx.

- **Unchanged lists:** if nothing changed, nothing is rewritten, and the run prints `no changes in /usr/local/nginx/conf/aicrawlers`.
- **Failed `nginx -t`:** the previous files are put back.
- **A list that fails to download or validate:** it is replaced by its last good copy.
- **A vendor with no good copy:** it is left out. Its crawlers get an empty verdict and keep their old botlimit.conf level.

Check what is installed:

```bash
/usr/local/src/centminmod/tools/nginx-aicrawlers.sh status
```

## Step 4: Choose a Level per AI Category

Edit the `$bot_agent_ai` map in `/usr/local/nginx/conf/botlimit.conf`:

```nginx
map $aicrawler_category $bot_agent_ai {
  default                    0;
  training                   0;   # collect content to train AI models: GPTBot, ClaudeBot, CCBot
  search                     0;   # AI search crawlers that link back: OAI-SearchBot, PerplexityBot, Applebot
  user                       0;   # fetches a person asked an AI assistant or agent for: ChatGPT-User, Claude-User
  other                      0;   # AI crawlers without a clear purpose
}
```

Set a category to `2` to rate limit it, or `3` to block it. A common choice is to block training crawlers and leave search and user fetches alone:

```nginx
  training                   3;
```

These levels apply only to user agents the main list leaves at 0. A user agent the list already whitelists or blocks keeps that level. Apply the change:

```bash
nginx -t && ngxreload
```

## Step 5: Apply the Levels in Your Vhosts

Levels 2 and 3 only take effect where a vhost uses them. Add the lines from the end of `botlimit.conf` inside the `server {}` or `location {}` blocks you want to protect, for example in `/usr/local/nginx/conf/conf.d/example.com.ssl.conf`:

```nginx
location / {
  # level 2: rate limited
  limit_conn botb_connlimit 10;
  limit_req  zone=botb_reqlimitip burst=10;
  # level 3: blocked (444 drops the connection, or use 403)
  if ($bot_agent = '3') {
    return 444;
  }
  ...
}
```

### Optional: block spoofed crawlers

Once the tool has run, you can block any user agent that claims to be a Google, Bing, OpenAI, Anthropic, Perplexity or Apple crawler from an IP outside that vendor's list:

```nginx
if ($aicrawler_verified = '0') {
  return 403;
}
```

Leave this out if you are behind a proxy without `real_ip` set up. Every real crawler would fail the check.

## Step 6: Keep the Lists Current

Vendors change their crawler IP ranges. Install a daily update:

```bash
/usr/local/src/centminmod/tools/nginx-aicrawlers.sh cron-install
```

This writes `/etc/cron.d/centminmod-aicrawlers`, which runs `update` once a day at 05:MM. The minute comes from the server's hostname, so different servers download at different minutes. `status` shows the last run. To remove the cron job:

```bash
/usr/local/src/centminmod/tools/nginx-aicrawlers.sh cron-remove
```

## Settings

Override the defaults in `/etc/centminmod/custom_config.inc`:

| Setting | Default | Purpose |
|---------|---------|---------|
| `AICRAWLERS_DIR` | `/usr/local/nginx/conf/aicrawlers` | where the map files are written |
| `AICRAWLERS_ROBOTS_URL` | ai.robots.txt `robots.json` on GitHub | user agent list source |
| `AICRAWLERS_SKIP` | empty | extra user agent tokens to leave out, comma separated, e.g. `'Cursor,Devin'` |
| `AICRAWLERS_RELOAD` | `y` | reload nginx after an update changed the files |

## Troubleshooting

### A normal client is treated as an AI crawler

Some tokens in `robots.json` are short (`Dot`, `Cursor`, `Devin`, `Operator`, `Muse`, `Trae`) and can match other software. `Code` is always left out because it matches VS Code's user agent. Leave other tokens out:

```bash
echo "AICRAWLERS_SKIP='Cursor,Devin'" >> /etc/centminmod/custom_config.inc
/usr/local/src/centminmod/tools/nginx-aicrawlers.sh update
```

### Real Googlebot is rate limited

Googlebot is only rate limited when Google's lists loaded and the client address is not in them. Most often `$remote_addr` is your proxy's or CDN's IP rather than the visitor's, so set up `real_ip` for it.

To see which vendor lists loaded, run `dry-run` and look for a `vendor   google` line with a count of IP prefixes. A vendor whose list failed shows `left out (no usable list)`.

### `update` reports a list failed

That vendor keeps its last good copy. If it has never downloaded, the vendor is left out and its crawlers keep their previous level. Run `update` again later. Lists that fail validation (bad IP ranges, too few user agents) are rejected on purpose.

### `nginx -t` fails with `open() ".../aicrawlers/aicrawlers_ua.conf" failed`

botlimit.conf includes the map files, but they are missing. Copy the empty files as shown in Step 1, then run `nginx -t` again.

## Related Guides

- [Anubis Bot Challenge](nginx-anubis.md): proof-of-work challenge for browsers
- [Nginx Modules in 141.00beta01](nginx-141-modules.md)
