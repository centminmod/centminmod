# Centmin Mod User Guides

Practical guides for common Centmin Mod configurations and workflows.

---

## Quick Navigation

| Guide | Category | Created | Updated |
|-------|----------|---------|---------|
| [SSL Certificate Multi-Server Setup](ssl-letsencrypt-multi-server.md) | SSL & HTTPS | 2026-01-31 | 2026-01-31 |
| [Nginx Docker Reverse Proxy](nginx-docker-reverse-proxy.md) | Nginx & Docker | 2026-01-31 | 2026-01-31 |
| [Nginx Modules in 141.00beta01](nginx-141-modules.md) | Nginx | 2026-11-09 | 2026-11-09 |
| [AI Crawler Maps for Nginx](nginx-ai-crawlers.md) | Nginx & Bots | 2026-11-09 | 2026-11-09 |
| [Anubis Bot Challenge for Nginx](nginx-anubis.md) | Nginx & Bots | 2026-11-09 | 2026-11-09 |
| [MCP / SSE / LLM Reverse Proxy](nginx-mcp-proxy.md) | Nginx & AI | 2026-11-09 | 2026-11-09 |

---

## Guide Details

### SSL Certificate Multi-Server Setup

**File:** [ssl-letsencrypt-multi-server.md](ssl-letsencrypt-multi-server.md)

Run the same domain with valid Let's Encrypt SSL certificates on multiple Centmin Mod servers.

**Use cases covered:**
- Server migration (move site from old server to new server)
- Load balancing / high availability (multiple servers, same domain)
- Staging/testing (test on new server before DNS switch)
- Disaster recovery (backup server ready to take over)

**Topics include:**
- Decision tree for choosing the right certificate method
- Cloudflare DNS API validation setup for multi-server environments
- Certificate transfer between servers
- Using `reissue-only` vs `issue` commands
- Staging workflow with hosts file testing
- Troubleshooting certificate validation failures

---

### Nginx Docker Reverse Proxy

**File:** [nginx-docker-reverse-proxy.md](nginx-docker-reverse-proxy.md)

Configure Centmin Mod nginx as a reverse proxy for Docker containers.

**Use cases covered:**
- Running Docker applications (Fider, Grafana, Mattermost, etc.) behind nginx
- SSL/TLS termination for containerized apps
- WebSocket support for real-time applications
- CSF firewall integration with Docker

**Topics include:**
- Docker network architecture and port mapping best practices
- Essential proxy headers configuration
- WebSocket support setup
- Step-by-step Fider application example
- CSF firewall configuration for Docker (docker.sh csf-setup)
- Generic nginx reverse proxy templates
- Common Docker applications configuration (Node.js, Django, Grafana, Mattermost)
- Troubleshooting 502/504 errors, connection issues, and SSL problems

---

### Nginx Modules in 141.00beta01

**File:** [nginx-141-modules.md](nginx-141-modules.md)

What changed for nginx modules in Centmin Mod 141.00beta01, and how to turn on the new optional modules.

**Use cases covered:**
- Picking up the module security updates (njs, ngx_cache_purge, Angie, freenginx, ModSecurity)
- Running JavaScript on the QuickJS engine with njs
- JWT authentication, key-value lookups and Redis-backed rate limiting in nginx
- Prometheus metrics, JA4 fingerprints and other Lua libraries
- OpenTelemetry tracing and HTTP-FLV live streaming

**Topics include:**
- Turning optional modules on and off with custom_config.inc and a menu 4 rebuild
- Checking a build with nginx -V and dynamic-modules.conf
- The JSON module and Angie ACME client, both on by default
- Example configs for each optional module
- Moving off the deprecated sticky and JA3 modules
- Upgrade notes for existing servers

---

### AI Crawler Maps for Nginx

**File:** [nginx-ai-crawlers.md](nginx-ai-crawlers.md)

Sort AI crawlers by what they do, then rate limit or block each kind separately with tools/nginx-aicrawlers.sh and botlimit.conf.

**Use cases covered:**
- Blocking AI training crawlers while allowing AI search and user-requested fetches
- Catching fake Googlebot and bingbot traffic from IPs outside their published ranges
- Keeping crawler IP lists current with a daily update

**Topics include:**
- Updating an older botlimit.conf
- dry-run, update, status and cron-install
- Per-category levels and vhost rate limit and block lines
- Settings and troubleshooting false matches and proxies

---

### Anubis Bot Challenge for Nginx

**File:** [nginx-anubis.md](nginx-anubis.md)

Protect a site from scrapers with an Anubis proof-of-work challenge, using addons/anubis.sh and nginx auth_request.

**Use cases covered:**
- Stopping mass scrapers that don't run JavaScript
- Protecting a whole site or chosen paths
- Exempting APIs, feeds and webhooks

**Topics include:**
- Building nginx with auth_request
- Installing, checking status and uninstalling Anubis
- Adding the includes to a vhost
- Editing the bot policy and settings
- Troubleshooting 500 errors, challenge loops and location conflicts

---

### MCP / SSE / LLM Reverse Proxy

**File:** [nginx-mcp-proxy.md](nginx-mcp-proxy.md)

Put an MCP server, an SSE stream or a streaming LLM API behind nginx with the mcp_proxy.conf templates.

**Use cases covered:**
- Hosting a remote MCP server behind nginx with HTTPS
- Streaming LLM API responses without buffering
- Session affinity and load balancing for MCP and LLM backends

**Topics include:**
- The Origin allow list the MCP specification requires
- Upstream and location setup, and testing with curl
- Changing template settings safely
- Optional Mcp-Method and Mcp-Name header checks with the nginx JSON module
- Troubleshooting buffering, timeouts, 403 and 400 responses
