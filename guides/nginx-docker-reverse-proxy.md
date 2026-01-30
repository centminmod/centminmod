# Nginx Docker Reverse Proxy Guide

Comprehensive guide for configuring Centmin Mod nginx as a reverse proxy for Docker containers.

## Overview

This guide explains how to configure Centmin Mod's nginx to act as a reverse proxy for applications running in Docker containers. This is the recommended approach for running Docker applications alongside Centmin Mod's LEMP stack.

**Architecture:**
```
Internet → nginx (port 80/443) → Docker Container (internal port)
```

**Benefits:**
- SSL/TLS termination at nginx level (single certificate management)
- Leverage nginx's caching, compression, and security features
- Consistent logging and monitoring
- No port conflicts with existing nginx installation

## Prerequisites

1. **Docker CE Installed** via Centmin Mod's addon script:
   ```bash
   cd /usr/local/src/centminmod/addons
   ./docker.sh install
   ```

   **docker.sh Available Commands:**
   - `install` - Install Docker CE with official repository
   - `csf-setup` - Configure CSF firewall integration
   - `csf-test` - Test Docker-CSF integration
   - `network-info` - Show Docker network information
   - `inspect-logs` - Interactive Docker log inspector
   - `clean` - Remove all containers, images, networks, volumes
   - `uninstall` - Complete Docker removal and CSF cleanup

2. **CSF Firewall Integration** configured (automatic with docker.sh install)

3. **Nginx vhost created** for the target domain:
   ```bash
   cd /usr/local/src/centminmod
   ./centmin.sh 2  # Add Nginx vhost domain
   ```

## Docker Network Architecture

### Port Mapping Best Practices

**Critical Rule**: Never map Docker containers to ports 80 or 443 - these are used by nginx.

| Mapping | Description | Recommended |
|---------|-------------|-------------|
| `80:3000` | Host port 80 → Container port 3000 | **NO** - conflicts with nginx |
| `3000:3000` | Host port 3000 → Container port 3000 | **YES** - internal access |
| `127.0.0.1:3000:3000` | Localhost only → Container port 3000 | **BEST** - most secure |

### Network Modes

**Bridge Network (Default - Recommended for most cases)**
```yaml
services:
  app:
    ports:
      - "127.0.0.1:3000:3000"  # Only accessible from localhost
```

**Host Network (Use with caution)**
```yaml
services:
  app:
    network_mode: host  # Container shares host's network stack
```

### Docker Internal DNS

Containers on the same Docker network can communicate using service names:
```yaml
services:
  app:
    depends_on:
      - db
    environment:
      - DATABASE_URL=postgres://db:5432/myapp  # 'db' resolves to database container
  db:
    image: postgres:16
```

## Nginx Reverse Proxy Configuration

### Essential Proxy Headers

A working reverse proxy requires proper headers for the application to function correctly:

```nginx
# Required headers for reverse proxy
proxy_set_header Host $host;
proxy_set_header X-Real-IP $remote_addr;
proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
proxy_set_header X-Forwarded-Proto $scheme;
proxy_set_header X-Forwarded-Host $host;
proxy_set_header X-Forwarded-Port $server_port;

# Connection handling
proxy_http_version 1.1;
proxy_set_header Connection "";

# Timeouts
proxy_connect_timeout 60s;
proxy_send_timeout 60s;
proxy_read_timeout 60s;
```

### WebSocket Support

Many modern applications (chat apps, real-time features) require WebSocket support:

```nginx
# WebSocket support
map $http_upgrade $connection_upgrade {
    default upgrade;
    '' close;
}

server {
    location / {
        proxy_pass http://127.0.0.1:3000;

        # WebSocket headers
        proxy_http_version 1.1;
        proxy_set_header Upgrade $http_upgrade;
        proxy_set_header Connection $connection_upgrade;

        # Standard proxy headers
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
        proxy_set_header X-Forwarded-Proto $scheme;
    }
}
```

### Upstream Block (Optional but Recommended)

For better organization and potential load balancing:

```nginx
upstream docker_app {
    server 127.0.0.1:3000;
    keepalive 32;
}

server {
    location / {
        proxy_pass http://docker_app;
        # ... headers ...
    }
}
```

## Step-by-Step: Fider Application Example

Fider is a feedback collection platform that demonstrates common Docker reverse proxy requirements.

### Step 1: Fix Docker Compose Configuration

**Original (Problematic):**
```yaml
services:
  app:
    image: getfider/fider:stable
    ports:
      - "80:3000"  # WRONG: Conflicts with nginx
    environment:
      BASE_URL: http://feedback.example.com  # WRONG: Should be HTTPS
```

**Corrected:**
```yaml
version: '3.8'

services:
  app:
    image: getfider/fider:stable
    restart: unless-stopped
    ports:
      - "127.0.0.1:3000:3000"  # Only accessible from localhost
    depends_on:
      - db
    environment:
      # Application
      BASE_URL: https://feedback.example.com  # Use actual domain with HTTPS

      # Database (use service name 'db' for internal DNS)
      DATABASE_URL: postgres://fider:your_secure_password@db:5432/fider?sslmode=disable

      # Security
      JWT_SECRET: your_jwt_secret_here_minimum_32_chars

      # Email (adjust for your SMTP provider)
      EMAIL_NOREPLY: noreply@example.com
      EMAIL_SMTP_HOST: smtp.example.com
      EMAIL_SMTP_PORT: 587
      EMAIL_SMTP_USERNAME: your_smtp_user
      EMAIL_SMTP_PASSWORD: your_smtp_password
      EMAIL_SMTP_ENABLE_STARTTLS: "true"

  db:
    image: postgres:16
    restart: unless-stopped
    volumes:
      - fider_postgres_data:/var/lib/postgresql/data
    environment:
      POSTGRES_USER: fider
      POSTGRES_PASSWORD: your_secure_password
      POSTGRES_DB: fider

volumes:
  fider_postgres_data:
```

### Step 2: Create Nginx Vhost Configuration

Create the vhost using Centmin Mod's menu option 2, then modify the configuration.

**Location:** `/usr/local/nginx/conf/conf.d/feedback.example.com.ssl.conf`

**Complete Configuration:**
```nginx
# WebSocket upgrade map (add to http context or use existing)
map $http_upgrade $connection_upgrade {
    default upgrade;
    '' close;
}

# Upstream for Fider Docker container
upstream fider_backend {
    server 127.0.0.1:3000;
    keepalive 32;
}

server {
    listen 443 ssl http2;
    server_name feedback.example.com;

    # SSL certificates (managed by Centmin Mod/acme.sh)
    ssl_certificate /usr/local/nginx/conf/ssl/feedback.example.com/feedback.example.com-bundle.crt;
    ssl_certificate_key /usr/local/nginx/conf/ssl/feedback.example.com/feedback.example.com.key;

    # SSL configuration (Centmin Mod defaults)
    include /usr/local/nginx/conf/ssl_include.conf;

    # Logging
    access_log /home/nginx/domains/feedback.example.com/log/access.log combined buffer=256k flush=5m;
    error_log /home/nginx/domains/feedback.example.com/log/error.log;

    # Security headers
    add_header X-Frame-Options "SAMEORIGIN" always;
    add_header X-Content-Type-Options "nosniff" always;
    add_header X-XSS-Protection "1; mode=block" always;
    add_header Referrer-Policy "strict-origin-when-cross-origin" always;

    location / {
        proxy_pass http://fider_backend;

        # Essential proxy headers
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
        proxy_set_header X-Forwarded-Proto $scheme;
        proxy_set_header X-Forwarded-Host $host;
        proxy_set_header X-Forwarded-Port $server_port;

        # WebSocket support (Fider uses WebSockets for real-time updates)
        proxy_http_version 1.1;
        proxy_set_header Upgrade $http_upgrade;
        proxy_set_header Connection $connection_upgrade;

        # Timeouts
        proxy_connect_timeout 60s;
        proxy_send_timeout 60s;
        proxy_read_timeout 60s;

        # Buffering (adjust based on application needs)
        proxy_buffering on;
        proxy_buffer_size 4k;
        proxy_buffers 8 4k;
        proxy_busy_buffers_size 8k;
    }

    # Static assets caching (if applicable)
    location ~* \.(js|css|png|jpg|jpeg|gif|ico|svg|woff|woff2)$ {
        proxy_pass http://fider_backend;
        proxy_set_header Host $host;
        proxy_cache_valid 200 30d;
        expires 30d;
        add_header Cache-Control "public, immutable";
    }
}

# HTTP to HTTPS redirect
server {
    listen 80;
    server_name feedback.example.com;
    return 301 https://$server_name$request_uri;
}
```

### Step 3: Obtain SSL Certificate

Using Centmin Mod's acme.sh integration:

```bash
# Webroot method (domain must resolve to server)
/root/.acme.sh/acme.sh --issue -d feedback.example.com -w /home/nginx/domains/feedback.example.com/public

# Install certificate
/root/.acme.sh/acme.sh --install-cert -d feedback.example.com \
    --key-file /usr/local/nginx/conf/ssl/feedback.example.com/feedback.example.com.key \
    --fullchain-file /usr/local/nginx/conf/ssl/feedback.example.com/feedback.example.com-bundle.crt \
    --reloadcmd "systemctl reload nginx"
```

### Step 4: Test and Deploy

```bash
# Test nginx configuration
nginx -t

# Reload nginx
systemctl reload nginx

# Start Docker containers
cd /path/to/fider
docker compose up -d

# Verify container is running
docker compose ps

# Test connectivity from host
curl -I http://127.0.0.1:3000

# Test via nginx
curl -I https://feedback.example.com
```

## CSF Firewall Considerations

### Quick Setup with docker.sh

On Centmin Mod 141.00beta01 (EL10 development branch), the `addons/docker.sh` script provides automated CSF firewall configuration for Docker. If you installed Docker manually or need to fix CSF integration:

```bash
cd /usr/local/src/centminmod/addons
curl -sL https://github.com/centminmod/centminmod/raw/refs/heads/141.00beta01/addons/docker.sh -o docker.sh
chmod +x docker.sh
./docker.sh csf-setup
./docker.sh csf-test
```

**Note:** Results may vary depending on your Docker installation method and configuration. The `csf-setup` command configures iptables rules and CSF settings for Docker compatibility, while `csf-test` verifies the configuration.

### Docker Network Allowlisting

The `addons/docker.sh` script automatically configures CSF for Docker. Manual configuration if needed:

**`/etc/csf/csf.allow`:**
```
# Docker bridge network (default)
172.17.0.0/16

# Docker compose networks (check with: docker network ls)
172.18.0.0/16
172.19.0.0/16
```

### Keep Internal Ports Closed

Since nginx proxies traffic, Docker container internal ports should NOT be added to CSF's TCP_IN:

```bash
# DO NOT add Docker container ports (like 3000, 8065, etc.) to TCP_IN
# These ports are bound to 127.0.0.1 and only accessible locally via nginx proxy

# Your existing TCP_IN should already include standard ports - leave it as is
# Just don't add internal Docker ports like 3000 to it
# Example of ports that should NOT be in TCP_IN: 3000, 8000, 8065, etc.
```

**Important:** Do NOT modify your existing TCP_IN configuration. Keep all your current allowed ports. The point is simply to avoid adding Docker's internal ports (3000, 8065, etc.) since nginx handles external traffic on ports 80/443.

### Verify Docker-CSF Integration

```bash
# Check DOCKER_IPTABLES_LOAD in /etc/csf/csf.conf
grep DOCKER /etc/csf/csf.conf

# Restart CSF after changes
csf -r
```

## Generic Configuration Template

### Minimal Reverse Proxy Template

For simple applications without WebSocket requirements:

```nginx
upstream docker_app_name {
    server 127.0.0.1:PORT;
    keepalive 16;
}

server {
    listen 443 ssl http2;
    server_name app.example.com;

    ssl_certificate /path/to/cert.crt;
    ssl_certificate_key /path/to/cert.key;
    include /usr/local/nginx/conf/ssl_include.conf;

    access_log /home/nginx/domains/app.example.com/log/access.log combined buffer=256k flush=5m;
    error_log /home/nginx/domains/app.example.com/log/error.log;

    location / {
        proxy_pass http://docker_app_name;
        proxy_http_version 1.1;
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
        proxy_set_header X-Forwarded-Proto $scheme;
        proxy_set_header Connection "";

        proxy_connect_timeout 60s;
        proxy_send_timeout 60s;
        proxy_read_timeout 60s;
    }
}

server {
    listen 80;
    server_name app.example.com;
    return 301 https://$server_name$request_uri;
}
```

### Full Template with WebSocket Support

For applications with real-time features:

```nginx
map $http_upgrade $connection_upgrade {
    default upgrade;
    '' close;
}

upstream docker_app_name {
    server 127.0.0.1:PORT;
    keepalive 32;
}

server {
    listen 443 ssl http2;
    server_name app.example.com;

    ssl_certificate /path/to/cert.crt;
    ssl_certificate_key /path/to/cert.key;
    include /usr/local/nginx/conf/ssl_include.conf;

    access_log /home/nginx/domains/app.example.com/log/access.log combined buffer=256k flush=5m;
    error_log /home/nginx/domains/app.example.com/log/error.log;

    # Security headers
    add_header X-Frame-Options "SAMEORIGIN" always;
    add_header X-Content-Type-Options "nosniff" always;
    add_header X-XSS-Protection "1; mode=block" always;

    # Max upload size (adjust as needed)
    client_max_body_size 50M;

    location / {
        proxy_pass http://docker_app_name;
        proxy_http_version 1.1;

        # Standard headers
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
        proxy_set_header X-Forwarded-Proto $scheme;
        proxy_set_header X-Forwarded-Host $host;
        proxy_set_header X-Forwarded-Port $server_port;

        # WebSocket support
        proxy_set_header Upgrade $http_upgrade;
        proxy_set_header Connection $connection_upgrade;

        # Timeouts
        proxy_connect_timeout 60s;
        proxy_send_timeout 60s;
        proxy_read_timeout 60s;

        # Buffering
        proxy_buffering on;
        proxy_buffer_size 4k;
        proxy_buffers 8 4k;
    }

    # Health check endpoint (if application provides one)
    location /health {
        proxy_pass http://docker_app_name;
        proxy_set_header Host $host;
        access_log off;
    }
}

server {
    listen 80;
    server_name app.example.com;
    return 301 https://$server_name$request_uri;
}
```

## Common Docker Applications

### Node.js Applications

**docker-compose.yml:**
```yaml
services:
  node-app:
    build: .
    ports:
      - "127.0.0.1:3000:3000"
    environment:
      - NODE_ENV=production
      - PORT=3000
```

**Nginx considerations:**
- Usually require WebSocket support if using Socket.io
- May need increased `proxy_read_timeout` for long-polling

### Python/Django/Flask Applications

**docker-compose.yml:**
```yaml
services:
  django-app:
    build: .
    ports:
      - "127.0.0.1:8000:8000"
    command: gunicorn --bind 0.0.0.0:8000 myapp.wsgi
```

**Nginx considerations:**
- Static files often served separately
- May need `X-Script-Name` header for URL prefix deployments

### Grafana

**docker-compose.yml:**
```yaml
services:
  grafana:
    image: grafana/grafana:latest
    ports:
      - "127.0.0.1:3001:3000"
    volumes:
      - grafana_data:/var/lib/grafana
    environment:
      - GF_SERVER_ROOT_URL=https://grafana.example.com
      - GF_SERVER_SERVE_FROM_SUB_PATH=false
```

**Nginx specific configuration:**
```nginx
location / {
    proxy_pass http://127.0.0.1:3001;
    proxy_set_header Host $host;
    proxy_set_header X-Real-IP $remote_addr;
    proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
    proxy_set_header X-Forwarded-Proto $scheme;
}

# Grafana Live WebSocket
location /api/live/ {
    proxy_pass http://127.0.0.1:3001;
    proxy_http_version 1.1;
    proxy_set_header Upgrade $http_upgrade;
    proxy_set_header Connection $connection_upgrade;
    proxy_set_header Host $host;
}
```

### Mattermost

**docker-compose.yml:**
```yaml
services:
  mattermost:
    image: mattermost/mattermost-team-edition:latest
    ports:
      - "127.0.0.1:8065:8065"
    environment:
      - MM_SERVICESETTINGS_SITEURL=https://chat.example.com
```

**Nginx specific configuration:**
```nginx
location ~ /api/v[0-9]+/(users/)?websocket$ {
    proxy_pass http://127.0.0.1:8065;
    proxy_http_version 1.1;
    proxy_set_header Upgrade $http_upgrade;
    proxy_set_header Connection $connection_upgrade;
    proxy_set_header Host $host;
    proxy_set_header X-Real-IP $remote_addr;
    proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
    proxy_set_header X-Forwarded-Proto $scheme;
    proxy_read_timeout 600s;
}

location / {
    proxy_pass http://127.0.0.1:8065;
    proxy_set_header Host $host;
    proxy_set_header X-Real-IP $remote_addr;
    proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
    proxy_set_header X-Forwarded-Proto $scheme;
    proxy_set_header X-Frame-Options SAMEORIGIN;
    client_max_body_size 50M;
}
```

## Troubleshooting

### 502 Bad Gateway

**Causes and Solutions:**

1. **Container not running**
   ```bash
   docker compose ps
   docker compose logs app
   ```

2. **Wrong port in proxy_pass**
   ```bash
   # Check what port container is listening on
   docker compose port app 3000
   # Verify from host
   curl -I http://127.0.0.1:3000
   ```

3. **Port conflict (container mapped to 80)**
   ```bash
   # Check for port conflicts
   ss -tlnp | grep ':80'
   # Fix: Change docker-compose ports from "80:3000" to "127.0.0.1:3000:3000"
   ```

4. **Container only listening on 127.0.0.1 inside container**
   ```bash
   # Check container's listening address
   docker compose exec app ss -tlnp
   # Fix: Ensure app binds to 0.0.0.0 inside container
   ```

5. **SELinux blocking connection**
   ```bash
   # Check for SELinux denials
   ausearch -m avc -ts recent
   # Temporary fix (test only)
   setsebool -P httpd_can_network_connect 1
   ```

### 504 Gateway Timeout

**Causes and Solutions:**

1. **Application taking too long to respond**
   ```nginx
   # Increase timeouts
   proxy_connect_timeout 300s;
   proxy_send_timeout 300s;
   proxy_read_timeout 300s;
   ```

2. **Database connection issues**
   ```bash
   # Check database container
   docker compose logs db
   docker compose exec db pg_isready
   ```

### Connection Refused

**Diagnostic Steps:**

```bash
# 1. Verify container is running
docker compose ps

# 2. Check container logs
docker compose logs -f app

# 3. Test from inside container
docker compose exec app curl -I http://localhost:3000

# 4. Test from host to container
curl -I http://127.0.0.1:3000

# 5. Check nginx error log
tail -f /home/nginx/domains/example.com/log/error.log

# 6. Check if port is bound
ss -tlnp | grep 3000
```

### SSL/HTTPS Issues

1. **Mixed content warnings**
   - Ensure `X-Forwarded-Proto` header is set
   - Application must respect forwarded headers
   - Check application's `BASE_URL` or equivalent setting

2. **Redirect loops**
   - Application may be forcing HTTPS when nginx already handles it
   - Disable HTTPS redirect in application when behind proxy

3. **Certificate errors**
   - Verify certificate files exist and are readable
   - Check certificate chain is complete
   - Ensure certificate matches domain

### WebSocket Connection Failures

```bash
# Check if upgrade headers are being passed
curl -i -N -H "Connection: Upgrade" -H "Upgrade: websocket" \
    -H "Sec-WebSocket-Key: test" -H "Sec-WebSocket-Version: 13" \
    https://app.example.com/ws

# Verify map directive is in http context
nginx -T | grep -A2 "map.*http_upgrade"
```

### Container Health Checks

Add health checks to docker-compose for reliability:

```yaml
services:
  app:
    image: myapp
    healthcheck:
      test: ["CMD", "curl", "-f", "http://localhost:3000/health"]
      interval: 30s
      timeout: 10s
      retries: 3
      start_period: 40s
```

## Quick Reference

### Checklist for New Docker App Behind Nginx

- [ ] Docker ports mapped to `127.0.0.1:PORT:INTERNAL_PORT` (not `80:PORT`)
- [ ] Application's BASE_URL/SITE_URL set to `https://domain.com`
- [ ] Nginx vhost created for domain
- [ ] SSL certificate obtained and configured
- [ ] Proxy headers configured in nginx location block
- [ ] WebSocket support added if needed (Upgrade/Connection headers)
- [ ] `nginx -t` passes
- [ ] Container accessible via `curl http://127.0.0.1:PORT`
- [ ] Site accessible via `curl https://domain.com`

### Common Ports by Application

| Application | Default Internal Port |
|-------------|----------------------|
| Fider | 3000 |
| Grafana | 3000 |
| Node.js | 3000 |
| Mattermost | 8065 |
| Django/Gunicorn | 8000 |
| Flask | 5000 |
| GitLab | 80 (use 8080 for proxy) |
| Nextcloud | 80 (use 8080 for proxy) |
| WordPress | 80 (use 8080 for proxy) |

## Related Resources

### Centmin Mod Documentation
- [Centmin Mod Official Documentation](https://centminmod.com/)
- [Centmin Mod Community Forums](https://community.centminmod.com/)
- [Let's Encrypt SSL Integration](https://centminmod.com/letsencrypt-freessl.html)

### Key File Locations
- **Nginx vhost configs**: `/usr/local/nginx/conf/conf.d/`
- **SSL certificates**: `/usr/local/nginx/conf/ssl/{domain}/`
- **Vhost web root**: `/home/nginx/domains/{domain}/public/`
- **Vhost logs**: `/home/nginx/domains/{domain}/log/`
- **Docker script**: `/usr/local/src/centminmod/addons/docker.sh`
- **acmetool.sh**: `/usr/local/src/centminmod/addons/acmetool.sh`

### SSL Certificate Commands
```bash
# Issue staging certificate (for testing)
./addons/acmetool.sh issue domain.com

# Issue production certificate
./addons/acmetool.sh issue domain.com live

# Issue production + HTTPS redirect
./addons/acmetool.sh issue domain.com lived

# Renew specific certificate
./addons/acmetool.sh renew domain.com live

# Check certificate expiry dates
./addons/acmetool.sh checkdates
```

## Need Help?

If you have questions about this guide or run into issues, ask on the Centmin Mod Community Forums:

- **Docker & Other Web Apps:** [Other Web Apps Usage Forum](https://community.centminmod.com/forums/other-web-apps-usage.36/)
- **Nginx Configuration:** [Nginx, PHP-FPM, MariaDB MySQL Forum](https://community.centminmod.com/forums/nginx-php-fpm-mariadb-mysql.21/)
