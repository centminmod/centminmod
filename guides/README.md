# Centmin Mod User Guides

Practical guides for common Centmin Mod configurations and workflows.

---

## Quick Navigation

| Guide | Category | Created | Updated |
|-------|----------|---------|---------|
| [SSL Certificate Multi-Server Setup](ssl-letsencrypt-multi-server.md) | SSL & HTTPS | 2026-01-31 | 2026-01-31 |
| [Nginx Docker Reverse Proxy](nginx-docker-reverse-proxy.md) | Nginx & Docker | 2026-01-31 | 2026-01-31 |

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
