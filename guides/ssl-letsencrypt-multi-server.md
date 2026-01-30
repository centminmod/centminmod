# SSL Certificate Multi-Server Setup Guide

Running the Same Domain on Multiple Centmin Mod Servers with Let's Encrypt HTTPS.

## Quick Start: What This Guide Solves

**Goal:** Run the same domain (e.g., `example.com`) with valid Let's Encrypt SSL certificates on multiple Centmin Mod servers.

**Common Use Cases:**
- Server migration (move site from old server to new server)
- Load balancing / high availability (multiple servers, same domain)
- Staging/testing (test on new server before DNS switch)
- Disaster recovery (backup server ready to take over)

**The Challenge:** Let's Encrypt validates domain ownership by connecting to your server. Only the server that DNS points to can successfully issue a new certificate. Servers that DNS doesn't point to need certificates transferred or must wait until DNS switches to them.

### Quick Decision: Which Method Do I Use?

```
Is your domain on Cloudflare DNS?
├── YES → Use Cloudflare DNS API (easiest for multi-server)
│         Configure CF_Token + CF_Account_ID, then:
│         Does nginx HTTPS vhost already exist?
│         ├── NO  → ./acmetool.sh issue domain.com live        (creates vhost + cert)
│         └── YES → ./acmetool.sh reissue-only domain.com live (cert only, keeps vhost)
│
└── NO → Is DNS pointing to this server?
         ├── YES → Does nginx HTTPS vhost already exist?
         │         ├── NO  → ./acmetool.sh issue domain.com live        (creates vhost + cert)
         │         └── YES → ./acmetool.sh reissue-only domain.com live (cert only, keeps vhost)
         │
         └── NO → Does the server already have the SSL vhost configured?
                  ├── YES (with self-signed placeholder) → Use Method 3 after DNS switches
                  │   ./acmetool.sh reissue-only domain.com live
                  │
                  └── NO → Either:
                           • Method 1: Copy certs from server that has them
                           • Staging Workflow: Test with hosts file, then Method 2/3 after DNS switch
```

#### Decision Tree Explained

**Step 1: Check if your domain uses Cloudflare DNS**

- **YES - Domain is on Cloudflare DNS:**
  - This is the easiest path for multi-server setups
  - Cloudflare DNS API validation bypasses the need for DNS to point to your server
  - You can issue certificates on ANY server regardless of where DNS points
  - Requires one-time setup: add `CF_Token` and `CF_Account_ID` to `/etc/centminmod/custom_config.inc`
  - See [Cloudflare DNS API Validation Setup](#cloudflare-dns-api-validation-setup) for details

- **NO - Domain is NOT on Cloudflare DNS:**
  - You must use webroot HTTP validation (default)
  - Let's Encrypt needs to connect to your server via the domain name
  - This means DNS must point to the server issuing the certificate
  - Continue to Step 2

**Step 2: Check if DNS points to this server**

- **YES - DNS points to this server:**
  - Let's Encrypt can validate your domain via HTTP
  - You can issue certificates directly
  - Continue to Step 3 to choose the right command

- **NO - DNS does NOT point to this server:**
  - Let's Encrypt cannot validate via HTTP (requests go to wrong server)
  - You have two options:
    - **Method 1:** Copy existing certificates from the server that has them
    - **Staging Workflow:** Set up the site, test via hosts file override, then issue cert after DNS switches

**Step 3: Check if nginx HTTPS vhost already exists**

Check with: `ls /usr/local/nginx/conf/conf.d/domain.com.ssl.conf`

- **NO - Vhost does NOT exist:**
  - Use `./acmetool.sh issue domain.com live`
  - This creates both the nginx vhost configuration AND issues the certificate
  - Use this for fresh/new setups

- **YES - Vhost already exists:**
  - Use `./acmetool.sh reissue-only domain.com live`
  - This ONLY issues a new certificate without touching your nginx vhost
  - Preserves your existing nginx vhost configuration
  - Use this when initial SSL issuance failed and vhost has self-signed placeholder
  - Use this for certificate renewals on existing sites

#### Quick Reference: How to Check Vhost Status

```bash
DOMAIN="domain.com"

# Check if HTTPS vhost file exists
if [[ -f /usr/local/nginx/conf/conf.d/${DOMAIN}.ssl.conf ]]; then
    echo "HTTPS vhost EXISTS - use 'reissue-only'"
else
    echo "HTTPS vhost does NOT exist - use 'issue'"
fi

# Check current certificate issuer (if vhost exists)
echo | openssl s_client -connect ${DOMAIN}:443 -servername ${DOMAIN} 2>/dev/null | \
  openssl x509 -noout -issuer 2>/dev/null
# "O=Let's Encrypt" = valid Let's Encrypt cert
# "O=Centmin Mod" = self-signed placeholder (needs reissue-only)
```

### acmetool.sh Command Reference

| Command | Nginx Vhost Exists? | Creates/Modifies Vhost? | Use Case |
|---------|--------------------|-----------------------|----------|
| `issue` | NO | Creates new vhost | First-time SSL setup |
| `reissue-only` | YES | No (cert only) | Renew/issue cert, keep existing vhost |

### Three Methods Summary

| Method | What It Does | Vhost Exists? | DNS Required* | Modifies Vhost |
|--------|--------------|---------------|--------------|----------------|
| **Method 1** | Copy existing certs from another server | Either | No | No |
| **Method 2** | `issue` - Create new vhost + cert | No | Yes | Creates |
| **Method 3** | `reissue-only` - Cert only, keep vhost | Yes | Yes | No |

*\*DNS requirement bypassed with Cloudflare DNS API validation (see below)*

### Cloudflare Users: The Easy Path

**If your domain uses Cloudflare DNS, you can skip the DNS-pointing requirement entirely.**

Cloudflare DNS API validation lets acmetool.sh issue certificates on ANY server without DNS pointing to it. The validation happens via DNS TXT records that acme.sh creates automatically through Cloudflare's API.

**Benefits:**
- Issue certificates on multiple servers simultaneously
- No need to wait for DNS propagation
- Works with Cloudflare proxy (orange cloud) enabled
- Perfect for load balancing, failover, and staging setups

**Quick Setup:**
```bash
# Add to /etc/centminmod/custom_config.inc on EACH server:
LETSENCRYPT_DETECT='y'
CF_DNSAPI_GLOBAL='y'
CF_Token="YOUR_CF_API_TOKEN"
CF_Account_ID="YOUR_CF_ACCOUNT_ID"
```

Then issue certificates - no DNS changes needed:
```bash
cd /usr/local/src/centminmod/addons

# If nginx HTTPS vhost does NOT exist yet:
./acmetool.sh issue domain.com live

# If nginx HTTPS vhost already exists:
./acmetool.sh reissue-only domain.com live  # Cert only, keeps vhost unchanged
```

See [Cloudflare DNS API Validation Setup](#cloudflare-dns-api-validation-setup) for detailed instructions.

## Detailed Overview

This guide covers three approaches for getting valid Let's Encrypt SSL certificates on Centmin Mod servers, plus a staging workflow for testing before DNS migration.

### Scenario Reference Table

| Scenario | Recommended Method | Use Case |
|----------|-------------------|----------|
| **Same domain, new server** | Method 1 or 2 | Server upgrade/replacement |
| **Testing before DNS switch** | Staging workflow + Method 2/3 | Zero-downtime migration |
| **Certificate expiring soon** | Method 2 | Fresh certificate with 90-day validity |
| **Preserving renewal history** | Method 1 | Keep existing acme.sh configuration |
| **Failed initial SSL issuance** | Method 3 | Vhost exists with self-signed fallback |
| **Custom nginx config preservation** | Method 3 | Cert-only update, keep vhost unchanged |
| **Load balancing setup** | Method 1 | Same cert on multiple servers |

### When to Use Each Method

**Method 1 (Certificate Migration) - Copy existing certs:**
- DNS does NOT point to this server (or you want same cert everywhere)
- Source server has valid Let's Encrypt certificate with >30 days remaining
- Want identical certificates across multiple servers
- Want to preserve acme.sh renewal configuration

**Method 2 (Certificate Issuance) - Issue fresh certificate:**
- DNS points to this server (or using Cloudflare DNS API)
- **Use `issue`** when nginx HTTPS vhost does NOT exist yet
- **Use `reissue-only`** when nginx HTTPS vhost already exists
- Setting up new server after DNS migration
- Certificate on source server expiring soon
- Want clean setup without legacy configuration

**Method 3 (Reissue-Only) - Update cert without touching vhost:**
- DNS points to this server (or using Cloudflare DNS API)
- HTTPS nginx vhost already exists (with self-signed placeholder or custom config)
- Initial Let's Encrypt issuance failed (DNS wasn't ready at setup time)
- Want to preserve custom nginx vhost modifications
- Only need certificate update, not vhost reconfiguration

## Prerequisites

### Old Server Requirements
- SSH root access
- Existing WordPress site with Let's Encrypt SSL
- Access to certificate files and acme.sh data

### New Server Requirements
- Centmin Mod installed (140.00beta01+ or 141.00beta01)
- WordPress vhost created via menu option 22
- Domain DNS pointing to new server (for Method 2/3) OR hosts file override (for staging) OR Cloudflare DNS API configured

### Create WordPress Vhost on New Server

Before migration, create the WordPress vhost structure on the new server:

```bash
cd /usr/local/src/centminmod
./centmin.sh
# Select option 22: Add Wordpress Nginx vhost + Cache Plugin
# Follow prompts for domain setup
```

When prompted for SSL, select staging certificate initially for testing.

## Key File Locations

### SSL Certificate Files
```
/usr/local/nginx/conf/ssl/domain.com/
├── domain.com.crt                      # Self-signed certificate (initial)
├── domain.com.key                      # Private key (initial)
├── domain.com.csr                      # Certificate signing request
├── dhparam.pem                         # Diffie-Hellman parameters (2048-bit)
├── domain.com-acme.cer                 # RSA Let's Encrypt certificate
├── domain.com-acme.key                 # RSA private key
├── domain.com-fullchain-acme.key       # RSA full chain (OCSP stapling)
├── domain.com-acme-ecc.cer             # ECDSA certificate (if DUALCERTS='y')
├── domain.com-acme-ecc.key             # ECDSA private key
├── domain.com-fullchain-acme-ecc.key   # ECDSA full chain
└── domain.com.crt.key.conf             # nginx SSL include file
```

### Nginx Vhost Configuration
```
/usr/local/nginx/conf/conf.d/domain.com.conf       # HTTP vhost
/usr/local/nginx/conf/conf.d/domain.com.ssl.conf   # HTTPS vhost
```

### acme.sh Data Directories
```
/root/.acme.sh/acme.sh                  # acme.sh binary
/root/.acme.sh/domain.com/              # RSA certificate data
/root/.acme.sh/domain.com_ecc/          # ECC certificate data (if dual certs)
```

## Cloudflare DNS API Validation Setup

If your domain uses Cloudflare DNS, you can issue Let's Encrypt certificates on ANY server without requiring DNS to point to that server. This is the simplest approach for multi-server setups.

### How It Works

Instead of the default webroot HTTP validation (which requires Let's Encrypt to connect to your server via the domain), Cloudflare DNS API validation:

1. acmetool.sh connects to Cloudflare API using your API token
2. Creates a temporary DNS TXT record (`_acme-challenge.domain.com`)
3. Let's Encrypt reads the TXT record to validate domain ownership
4. Certificate is issued to your server
5. TXT record is automatically cleaned up

**Result:** You can issue certificates on Server A, Server B, Server C - all at the same time, without changing where DNS points.

### Requirements

- Domain must use Cloudflare for DNS (nameservers at Cloudflare)
- Cloudflare API Token with appropriate permissions
- All domains must be in the same Cloudflare account (or accessible via invited admin access)

### Step 1: Create Cloudflare API Token

1. Go to https://dash.cloudflare.com/profile/api-tokens
2. Click **Create Token**
3. Use **Custom Token** with these permissions:
   - **Zone - Zone** - Read (to list your zones)
   - **Zone - DNS** - Edit (to create/delete TXT records)
4. Set **Zone Resources** to: Include - All zones (or specific zones)
5. Click **Continue to summary** → **Create Token**
6. **Copy the token immediately** (shown only once)

### Step 2: Get Your Cloudflare Account ID

1. Go to any domain's dashboard in Cloudflare
2. Look at the right sidebar under **API** section
3. Copy the **Account ID** value

### Step 3: Configure Centmin Mod

Add these variables to `/etc/centminmod/custom_config.inc` on **each server**:

```bash
# Create file if it doesn't exist
mkdir -p /etc/centminmod

cat >> /etc/centminmod/custom_config.inc << 'EOF'
# Cloudflare DNS API for Let's Encrypt validation
LETSENCRYPT_DETECT='y'
CF_DNSAPI_GLOBAL='y'
CF_Token="your_cloudflare_api_token_here"
CF_Account_ID="your_cloudflare_account_id_here"
EOF

# Secure the file (contains API credentials)
chmod 600 /etc/centminmod/custom_config.inc
```

### Step 4: Issue Certificates

With Cloudflare DNS API configured, issue certificates (works regardless of where DNS points):

```bash
cd /usr/local/src/centminmod/addons

# NEW vhost (nginx HTTPS vhost does NOT exist):
./acmetool.sh issue domain.com live

# NEW vhost with www subdomain:
./acmetool.sh issue domain.com live www

# EXISTING vhost (nginx HTTPS vhost already exists):
./acmetool.sh reissue-only domain.com live  # Cert only, preserves vhost config
```

**Important:** `issue` will fail if the nginx HTTPS vhost already exists. Use `reissue-only` for existing vhosts.

### Multi-Server Example

With Cloudflare DNS API, you can set up the same domain on multiple servers:

```
Server A (Production - DNS points here, NEW setup):
  ./acmetool.sh issue domain.com live         ✓ Creates vhost + cert

Server B (Staging - DNS doesn't point here, NEW setup):
  ./acmetool.sh issue domain.com live         ✓ Creates vhost + cert

Server C (Disaster Recovery - vhost already exists from previous setup):
  ./acmetool.sh reissue-only domain.com live  ✓ Issues cert, keeps vhost
```

Each server gets its own valid Let's Encrypt certificate. Renewals happen independently on each server.

### When to Use Cloudflare DNS API vs Other Methods

| Scenario | Cloudflare DNS API | Other Methods |
|----------|-------------------|---------------|
| Domain behind Cloudflare proxy (orange cloud) | **Recommended** | May have issues |
| Multiple servers, same domain | **Recommended** | Use Method 1 (copy certs) |
| Single server migration | Works | Method 2 after DNS switch |
| Domain not on Cloudflare | Not available | Use Methods 1-3 |

### Cloudflare Full/Full Strict SSL Mode

If you're using Cloudflare's **Full** or **Full Strict** SSL mode (recommended), Cloudflare DNS API validation is the best approach because:

- Cloudflare proxy intercepts HTTP requests, which can interfere with webroot validation
- DNS API validation bypasses this entirely
- Your origin server gets a valid Let's Encrypt certificate
- Cloudflare validates the origin certificate in Full Strict mode

### Troubleshooting Cloudflare DNS API

**Error: "No domain found"**
- Verify CF_Account_ID is correct
- Ensure API token has access to the zone

**Error: "Permission denied"**
- Check API token permissions (needs Zone:Read and DNS:Edit)
- Verify token hasn't expired

**Error: "Rate limit"**
- Cloudflare API has rate limits
- Wait and retry, or check for duplicate requests

**Verify configuration:**
```bash
# Check config file exists and has values
grep -E "^CF_" /etc/centminmod/custom_config.inc

# Test with staging certificate first
./acmetool.sh issue domain.com test
```

## Method 1: Certificate Migration (Copy Existing Certs)

Use this method to transfer existing certificates and acme.sh renewal configuration.

### Step 1: Backup Files from Old Server

SSH to the old server and create a backup archive:

```bash
# Set your domain
DOMAIN="domain.com"

# Create backup directory
mkdir -p /root/ssl-migration-backup

# Backup SSL certificates
cp -a /usr/local/nginx/conf/ssl/${DOMAIN}/ /root/ssl-migration-backup/ssl/

# Backup nginx vhost configs
cp /usr/local/nginx/conf/conf.d/${DOMAIN}.conf /root/ssl-migration-backup/
cp /usr/local/nginx/conf/conf.d/${DOMAIN}.ssl.conf /root/ssl-migration-backup/

# Backup acme.sh renewal data
cp -a /root/.acme.sh/${DOMAIN}/ /root/ssl-migration-backup/acme-rsa/
cp -a /root/.acme.sh/${DOMAIN}_ecc/ /root/ssl-migration-backup/acme-ecc/ 2>/dev/null || true

# Create archive
cd /root
tar -czvf ssl-migration-${DOMAIN}.tar.gz ssl-migration-backup/
```

### Step 2: Transfer to New Server

From your local machine or directly between servers:

```bash
# Option A: Direct server-to-server transfer
rsync -avzP /root/ssl-migration-${DOMAIN}.tar.gz root@NEW_SERVER_IP:/root/

# Option B: Via local machine
rsync -avzP root@OLD_SERVER_IP:/root/ssl-migration-${DOMAIN}.tar.gz ./
rsync -avzP ssl-migration-${DOMAIN}.tar.gz root@NEW_SERVER_IP:/root/

# Option C: Direct transfer without archive (individual directories)
rsync -avzP /usr/local/nginx/conf/ssl/${DOMAIN}/ root@NEW_SERVER_IP:/root/ssl-migration-backup/ssl/
rsync -avzP /root/.acme.sh/${DOMAIN}/ root@NEW_SERVER_IP:/root/ssl-migration-backup/acme-rsa/
rsync -avzP /root/.acme.sh/${DOMAIN}_ecc/ root@NEW_SERVER_IP:/root/ssl-migration-backup/acme-ecc/
```

**rsync flags:**
- `-a` - Archive mode (preserves permissions, ownership, timestamps)
- `-v` - Verbose output
- `-z` - Compress during transfer
- `-P` - Show progress and allow resume of interrupted transfers

### Step 3: Install on New Server

SSH to the new server and restore:

```bash
DOMAIN="domain.com"

# Extract backup
cd /root
tar -xzvf ssl-migration-${DOMAIN}.tar.gz

# Stop nginx during restoration
systemctl stop nginx

# Restore SSL certificates (backup existing first)
if [[ -d /usr/local/nginx/conf/ssl/${DOMAIN} ]]; then
    mv /usr/local/nginx/conf/ssl/${DOMAIN} /usr/local/nginx/conf/ssl/${DOMAIN}.bak
fi
cp -a /root/ssl-migration-backup/ssl/ /usr/local/nginx/conf/ssl/${DOMAIN}/

# Set proper permissions
chown -R nginx:nginx /usr/local/nginx/conf/ssl/${DOMAIN}/
chmod 600 /usr/local/nginx/conf/ssl/${DOMAIN}/*.key

# Restore acme.sh data (backup existing first)
if [[ -d /root/.acme.sh/${DOMAIN} ]]; then
    mv /root/.acme.sh/${DOMAIN} /root/.acme.sh/${DOMAIN}.bak
fi
cp -a /root/ssl-migration-backup/acme-rsa/ /root/.acme.sh/${DOMAIN}/

if [[ -d /root/ssl-migration-backup/acme-ecc ]]; then
    if [[ -d /root/.acme.sh/${DOMAIN}_ecc ]]; then
        mv /root/.acme.sh/${DOMAIN}_ecc /root/.acme.sh/${DOMAIN}_ecc.bak
    fi
    cp -a /root/ssl-migration-backup/acme-ecc/ /root/.acme.sh/${DOMAIN}_ecc/
fi
```

### Step 4: Update Vhost Configuration (If Needed)

If the nginx vhost configuration differs between servers, you may need to update paths:

```bash
# Compare configurations
diff /root/ssl-migration-backup/${DOMAIN}.ssl.conf /usr/local/nginx/conf/conf.d/${DOMAIN}.ssl.conf

# If needed, copy the SSL configuration
# (Usually not necessary if vhost was created via menu option 22)
```

### Step 5: Test and Restart

```bash
# Test nginx configuration
nginx -t

# If test passes, start nginx
systemctl start nginx

# Verify SSL is working
curl -vI https://${DOMAIN} 2>&1 | grep -E "(SSL|subject|expire)"
```

### Step 6: Verify acme.sh Renewal

```bash
# List registered domains
/root/.acme.sh/acme.sh --list

# Check certificate dates
cd /usr/local/src/centminmod/addons
./acmetool.sh checkdates

# Test renewal (dry run)
/root/.acme.sh/acme.sh --renew -d ${DOMAIN} --force --dry-run
```

## Method 2: Certificate Issuance (Fresh Certs - New Vhost)

Use this method to issue fresh Let's Encrypt certificates and create a new nginx vhost on the server.

**Prerequisites:**
- DNS must be pointing to the new server (unless using Cloudflare DNS API)
- nginx HTTPS vhost must NOT already exist (use Method 3 for existing vhosts)

### Step 1: Ensure acme.sh is Installed

```bash
cd /usr/local/src/centminmod/addons
./acmetool.sh acmeinstall
```

### Step 2: Check If Vhost Exists

```bash
DOMAIN="domain.com"

# Check if HTTPS vhost exists
ls -la /usr/local/nginx/conf/conf.d/${DOMAIN}.ssl.conf

# If file exists → use Method 3 (reissue-only) instead
# If file does NOT exist → continue with issue command below
```

### Step 3: Issue Certificate

```bash
DOMAIN="domain.com"
cd /usr/local/src/centminmod/addons

# Option A: Issue certificate (creates HTTP + HTTPS vhost)
./acmetool.sh issue ${DOMAIN} live

# Option B: Issue certificate with HTTPS as default
./acmetool.sh issue ${DOMAIN} lived

# Option C: Issue with www subdomain
./acmetool.sh issue ${DOMAIN} live www

# Option D: Issue staging cert for testing (not trusted by browsers)
./acmetool.sh issue ${DOMAIN} test
```

**Note:** If the nginx HTTPS vhost already exists, `issue` will fail. Use Method 3 (`reissue-only`) instead.

### Step 4: Verify Certificate Installation

```bash
# Test nginx configuration
nginx -t

# Reload nginx
systemctl reload nginx

# Check certificate details
openssl s_client -connect ${DOMAIN}:443 -servername ${DOMAIN} 2>/dev/null | openssl x509 -noout -dates -subject

# Verify with curl
curl -vI https://${DOMAIN} 2>&1 | head -30
```

### Reference: Centmin Mod HTTPS Migration Guide

For additional details, see the official Centmin Mod guide:
- https://centminmod.com/migrating-to-https.html (Steps 1-4)

## Method 3: Reissue-Only (Existing Vhost with Failed SSL)

Use this method when an HTTPS vhost already exists but has a self-signed certificate (fallback from failed Let's Encrypt issuance). This reissues the Let's Encrypt certificate **without modifying the nginx vhost configuration**.

### When to Use This Method

- Initial Let's Encrypt SSL issuance failed during vhost creation
- Vhost has self-signed SSL placeholder certificate
- DNS was not ready during initial setup but is now pointing correctly
- Want to upgrade from self-signed to Let's Encrypt without touching nginx config
- Migration scenario where vhost config is already correct but cert needs reissuing

### Prerequisites

- Existing `domain.com.ssl.conf` nginx vhost file
- DNS pointing to the server (required for Let's Encrypt validation)
- acme.sh installed (`./acmetool.sh acmeinstall`)

### Step 1: Verify Existing Vhost

```bash
DOMAIN="domain.com"

# Check that SSL vhost exists
ls -la /usr/local/nginx/conf/conf.d/${DOMAIN}.ssl.conf

# Check current certificate (likely self-signed)
openssl s_client -connect ${DOMAIN}:443 -servername ${DOMAIN} 2>/dev/null | \
  openssl x509 -noout -issuer -subject
# Self-signed will show: Issuer: ... O=Centmin Mod ...
```

### Step 2: Reissue Certificate Only

```bash
cd /usr/local/src/centminmod/addons

# Production certificate (does NOT modify nginx vhost)
./acmetool.sh reissue-only ${DOMAIN} live

# With www subdomain
./acmetool.sh reissue-only ${DOMAIN} live www

# Staging certificate for testing first
./acmetool.sh reissue-only ${DOMAIN} test
```

### Step 3: Verify New Certificate

```bash
# Reload nginx to pick up new certificate
systemctl reload nginx

# Verify Let's Encrypt certificate is now active
openssl s_client -connect ${DOMAIN}:443 -servername ${DOMAIN} 2>/dev/null | \
  openssl x509 -noout -issuer -dates
# Should show: Issuer: ... O=Let's Encrypt ...

# Check certificate files were created
ls -la /usr/local/nginx/conf/ssl/${DOMAIN}/*acme*
```

### Comparison: `issue` vs `reissue-only`

| Command | Modifies Nginx Vhost | Use Case |
|---------|---------------------|----------|
| `./acmetool.sh issue domain.com live` | Yes | New vhost or full reconfiguration |
| `./acmetool.sh reissue-only domain.com live` | No | Existing vhost, cert-only update |

### Migration Use Case

For server migration where you've already:
1. Created the WordPress vhost on new server (menu option 22)
2. Migrated WordPress files and database
3. Configured nginx vhost correctly
4. But initial SSL issuance failed (DNS wasn't ready)

After DNS propagation:
```bash
cd /usr/local/src/centminmod/addons
./acmetool.sh reissue-only domain.com live
systemctl reload nginx
```

This preserves all your nginx configuration customizations while only updating the SSL certificate.

## Staging Server Testing Workflow

This workflow allows testing the migration on the new server before switching DNS.

### Phase 1: Setup New Server with Staging Certificate

1. **Create WordPress vhost on new server:**
   ```bash
   cd /usr/local/src/centminmod
   ./centmin.sh
   # Select option 22, choose staging/self-signed SSL initially
   ```

2. **Migrate WordPress database:**
   ```bash
   # On old server: export database
   mysqldump -u root -p wordpress_db > /root/wordpress_db.sql

   # Transfer to new server
   rsync -avzP /root/wordpress_db.sql root@NEW_SERVER:/root/

   # On new server: import database
   mysql -u root -p wordpress_db < /root/wordpress_db.sql
   ```

3. **Migrate WordPress files:**
   ```bash
   # On old server: create archive
   tar -czvf /root/wp-files.tar.gz /home/nginx/domains/${DOMAIN}/public/

   # Transfer and extract on new server
   rsync -avzP /root/wp-files.tar.gz root@NEW_SERVER:/root/
   tar -xzvf /root/wp-files.tar.gz -C /
   chown -R nginx:nginx /home/nginx/domains/${DOMAIN}/public/

   # Alternative: Direct rsync without archive (faster for large sites)
   rsync -avzP --delete /home/nginx/domains/${DOMAIN}/public/ \
     root@NEW_SERVER:/home/nginx/domains/${DOMAIN}/public/
   ```

### Phase 2: Test via Hosts File Override

Edit your local hosts file to point the domain to the new server IP:

**Linux/Mac:**
```bash
sudo nano /etc/hosts
# Add line:
NEW_SERVER_IP domain.com www.domain.com
```

**Windows:**
```
# Edit C:\Windows\System32\drivers\etc\hosts as Administrator
# Add line:
NEW_SERVER_IP domain.com www.domain.com
```

**Testing:**
```bash
# Clear DNS cache (if needed)
# Mac: sudo dscacheutil -flushcache
# Windows: ipconfig /flushdns

# Test site access (will show certificate warning due to staging cert)
curl -k -vI https://domain.com

# Test in browser (accept certificate warning to view site)
```

### Phase 3: Switch DNS and Issue Production Certificate

1. **Remove hosts file entry:**
   ```bash
   sudo nano /etc/hosts
   # Remove or comment out the test line
   ```

2. **Update DNS records:**
   - Log into your DNS provider
   - Update A record to point to new server IP
   - Update AAAA record if using IPv6

3. **Wait for DNS propagation:**
   ```bash
   # Check DNS propagation
   dig +short domain.com
   nslookup domain.com

   # Or use online tools like:
   # https://www.whatsmydns.net/
   ```

4. **Issue Let's Encrypt certificate:**
   ```bash
   cd /usr/local/src/centminmod/addons
   ./acmetool.sh issue domain.com lived
   ```

5. **Verify production certificate:**
   ```bash
   curl -vI https://domain.com 2>&1 | grep -E "(SSL|subject|expire|issuer)"
   ```

## WordPress-Specific acmetool.sh Parameters

Menu option 22 uses special parameters when calling acmetool.sh:

| Parameter | Description | Certificate Type |
|-----------|-------------|------------------|
| `wptest` | WordPress staging cert, HTTP + HTTPS | Let's Encrypt Staging |
| `wptestd` | WordPress staging cert, HTTPS default | Let's Encrypt Staging |
| `wplive` | WordPress live cert, HTTP + HTTPS | Let's Encrypt Production |
| `wplived` | WordPress live cert, HTTPS default | Let's Encrypt Production |

These parameters are used internally by wpsetup.inc (lines 3300-3342) and include WordPress-specific webroot paths.

## Troubleshooting

### Certificate Not Trusted (Staging Certificate Used)

**Symptom:** Browser shows "Not Secure" or certificate warning
**Cause:** Let's Encrypt staging certificate was installed instead of production

**Solution:**
```bash
# Reissue with production certificate
cd /usr/local/src/centminmod/addons
./acmetool.sh issue domain.com lived
```

### DNS Not Propagated Yet

**Symptom:** acmetool.sh fails with validation error
**Cause:** Let's Encrypt cannot reach your server via the domain

**Solution:**
```bash
# Check DNS resolution
dig +short domain.com
nslookup domain.com

# Should return your new server IP
# If not, wait for propagation (up to 48 hours) or check DNS settings
```

### Let's Encrypt Rate Limits

**Symptom:** Error about too many certificates or rate limit exceeded
**Cause:** Let's Encrypt limits: 50 certificates per domain per week

**Solution:**
- Wait for rate limit reset (weekly)
- Use staging certificates for testing: `./acmetool.sh issue domain.com test`
- See: https://letsencrypt.org/docs/rate-limits/

### File Permission Issues

**Symptom:** nginx fails to start, permission denied errors
**Cause:** Certificate files have incorrect ownership/permissions

**Solution:**
```bash
DOMAIN="domain.com"
chown -R nginx:nginx /usr/local/nginx/conf/ssl/${DOMAIN}/
chmod 600 /usr/local/nginx/conf/ssl/${DOMAIN}/*.key
chmod 644 /usr/local/nginx/conf/ssl/${DOMAIN}/*.cer
chmod 644 /usr/local/nginx/conf/ssl/${DOMAIN}/*.crt
```

### Nginx Configuration Errors

**Symptom:** `nginx -t` fails
**Cause:** SSL paths incorrect or missing files

**Solution:**
```bash
# Check SSL paths in vhost
grep -E "ssl_certificate" /usr/local/nginx/conf/conf.d/${DOMAIN}.ssl.conf

# Verify files exist
ls -la /usr/local/nginx/conf/ssl/${DOMAIN}/

# Check nginx error log
tail -50 /var/log/nginx/error.log
```

### acme.sh Renewal Not Working

**Symptom:** Certificates not auto-renewing
**Cause:** acme.sh cron not set up or renewal configuration missing

**Solution:**
```bash
# Check acme.sh cron
crontab -l | grep acme

# Reinstall acme.sh cron
/root/.acme.sh/acme.sh --install-cronjob

# Check renewal configuration
cat /root/.acme.sh/${DOMAIN}/${DOMAIN}.conf
```

### Mixed Content After Migration

**Symptom:** HTTPS page loads but with warnings, images/CSS broken
**Cause:** WordPress URLs still pointing to HTTP or old server

**Solution:**
```bash
# Update WordPress site URL via WP-CLI
cd /home/nginx/domains/${DOMAIN}/public
wp option update siteurl 'https://domain.com' --allow-root
wp option update home 'https://domain.com' --allow-root

# Search and replace old URLs in database
wp search-replace 'http://domain.com' 'https://domain.com' --allow-root
wp search-replace 'http://www.domain.com' 'https://www.domain.com' --allow-root
```

## Verification Commands

### Test Nginx Configuration
```bash
nginx -t
```

### Check SSL Certificate Details
```bash
DOMAIN="domain.com"
openssl s_client -connect ${DOMAIN}:443 -servername ${DOMAIN} 2>/dev/null | \
  openssl x509 -noout -dates -subject -issuer
```

### Verify HTTPS Response
```bash
curl -vI https://domain.com 2>&1 | head -40
```

### Check Certificate Expiry Dates
```bash
cd /usr/local/src/centminmod/addons
./acmetool.sh checkdates
```

### List acme.sh Registered Domains
```bash
/root/.acme.sh/acme.sh --list
```

### Test acme.sh Renewal (Dry Run)
```bash
/root/.acme.sh/acme.sh --renew -d domain.com --force --dry-run
```

### Verify Certificate Chain
```bash
openssl s_client -connect domain.com:443 -servername domain.com 2>/dev/null | \
  openssl x509 -noout -text | grep -E "(Issuer|Subject|Not Before|Not After)"
```

## Related Resources

### Centmin Mod Documentation
- [Centmin Mod Official Documentation](https://centminmod.com/)
- [Let's Encrypt SSL Integration Guide](https://centminmod.com/letsencrypt-freessl.html)
- [Centmin Mod Community Forums](https://community.centminmod.com/)

### Key File Locations
- **acmetool.sh**: `/usr/local/src/centminmod/addons/acmetool.sh`
- **acme.sh client**: `/root/.acme.sh/acme.sh`
- **SSL certificates**: `/usr/local/nginx/conf/ssl/{domain}/`
- **Nginx vhost configs**: `/usr/local/nginx/conf/conf.d/`
- **Certificate backups**: `/root/.acme.sh/{domain}/`

### Certificate Management Quick Reference
```bash
# Issue staging certificate
./addons/acmetool.sh issue domain.com

# Issue production certificate
./addons/acmetool.sh issue domain.com live

# Force reissue certificate
./addons/acmetool.sh reissue domain.com live

# Renew all certificates
./addons/acmetool.sh renewall live

# Check certificate dates
./addons/acmetool.sh checkdates
```

## Need Help?

If you have questions about this guide or run into issues, ask on the Centmin Mod Community Forums:

- **SSL & Certificates:** [Domains, DNS, Email, SSL Certificates Forum](https://community.centminmod.com/forums/domains-dns-email-ssl-certificates.44/)
- **Nginx Configuration:** [Nginx, PHP-FPM, MariaDB MySQL Forum](https://community.centminmod.com/forums/nginx-php-fpm-mariadb-mysql.21/)
