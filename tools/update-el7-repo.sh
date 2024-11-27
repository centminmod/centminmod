#!/bin/bash
##########################################################################
# CentOS 7 is now EOL as of June 30, 2024
# Handle standard CentOS mirrors, Linode mirrors, and OVH mirrors
# Switch all to archive vault.centos.org
##########################################################################

# Function to update standard repository configuration files
update_repo_file() {
    local repo_file="$1"
    # Skip if file contains Linode or OVH mirrors
    if grep -q 'mirrors.linode.com/centos\|centos.mirrors.ovh.net' "$repo_file"; then
        return
    fi

    if grep -q 'mirrorlist=http://mirrorlist.centos.org' "$repo_file"; then
        sed -i 's|^mirrorlist=http://mirrorlist.centos.org.*|#mirrorlist=http://mirrorlist.centos.org|' "$repo_file"
        sed -i 's|^#baseurl=http://mirror.centos.org.*|baseurl=http://vault.centos.org/centos/$releasever/os/$basearch|' "$repo_file"
        sed -i 's|^mirrorlist=http://mirrorlist.centos.org/?release=\$releasever&arch=\$basearch&repo=os&infra=\$infra|baseurl=http://vault.centos.org/7.9.2009/os/\$basearch/|' "$repo_file"
        sed -i 's|^mirrorlist=http://mirrorlist.centos.org/?release=\$releasever&arch=\$basearch&repo=updates&infra=\$infra|baseurl=http://vault.centos.org/7.9.2009/updates/\$basearch/|' "$repo_file"
        sed -i 's|^mirrorlist=http://mirrorlist.centos.org/?release=\$releasever&arch=\$basearch&repo=extras&infra=\$infra|baseurl=http://vault.centos.org/7.9.2009/extras/\$basearch/|' "$repo_file"
        sed -i 's|^mirrorlist=http://mirrorlist.centos.org/?release=\$releasever&arch=\$basearch&repo=centosplus&infra=\$infra|baseurl=http://vault.centos.org/7.9.2009/centosplus/\$basearch/|' "$repo_file"
        sed -i 's|^mirrorlist=http://mirrorlist.centos.org/?release=\$releasever&arch=\$basearch&repo=fasttrack&infra=\$infra|baseurl=http://vault.centos.org/7.9.2009/fasttrack/\$basearch/|' "$repo_file"
        sed -i 's|^mirrorlist=http://mirrorlist.centos.org/?release=\$releasever&arch=\$basearch&repo=kernel&infra=\$infra|baseurl=http://vault.centos.org/7.9.2009/kernel/\$basearch/|' "$repo_file"
        sed -i 's|^mirrorlist=http://mirrorlist.centos.org/?release=\$releasever&arch=\$basearch&repo=experimental&infra=\$infra|baseurl=http://vault.centos.org/7.9.2009/experimental/\$basearch/|' "$repo_file"
        sed -i 's|^mirrorlist=http://mirrorlist.centos.org?arch=\$basearch&release=7&repo=sclo-rh|baseurl=http://vault.centos.org/7.9.2009/sclo/rh/\$basearch/|' "$repo_file"
        sed -i 's|^mirrorlist=http://mirrorlist.centos.org?arch=\$basearch&release=7&repo=sclo-sclo|baseurl=http://vault.centos.org/7.9.2009/sclo/sclo/\$basearch/|' "$repo_file"
    elif grep -q 'mirrorlist=http://mirrorlist.centos.org' "$repo_file"; then
        # Handle simpler mirror configurations
        sed -i 's|^mirrorlist=http://mirrorlist.centos.org.*|#mirrorlist=http://mirrorlist.centos.org|' "$repo_file"
        sed -i 's|^#baseurl=http://mirror.centos.org.*|baseurl=http://vault.centos.org/centos/$releasever/os/$basearch|' "$repo_file"
    fi
}

# Function to update Linode mirror configuration
update_linode_repo() {
    local repo_file="$1"
    if grep -q 'mirrors.linode.com/centos' "$repo_file"; then
        echo "Detected Linode mirror in $repo_file - updating..."
        sed -i 's|^baseurl=http://mirrors.linode.com/centos/\$releasever/os/\$basearch/|baseurl=http://vault.centos.org/7.9.2009/os/\$basearch/|' "$repo_file"
        sed -i 's|^baseurl=http://mirrors.linode.com/centos/\$releasever/updates/\$basearch/|baseurl=http://vault.centos.org/7.9.2009/updates/\$basearch/|' "$repo_file"
        sed -i 's|^baseurl=http://mirrors.linode.com/centos/\$releasever/extras/\$basearch/|baseurl=http://vault.centos.org/7.9.2009/extras/\$basearch/|' "$repo_file"
        sed -i 's|^baseurl=http://mirrors.linode.com/centos/\$releasever/centosplus/\$basearch/|baseurl=http://vault.centos.org/7.9.2009/centosplus/\$basearch/|' "$repo_file"
    fi
}

# Function to update OVH mirror configuration
update_ovh_repo() {
    local repo_file="$1"
    if grep -q 'centos.mirrors.ovh.net' "$repo_file"; then
        echo "Detected OVH mirror in $repo_file - updating..."
        sed -i 's|^baseurl=http://centos.mirrors.ovh.net/ftp.centos.org/7/os/\$basearch/|baseurl=http://vault.centos.org/7.9.2009/os/\$basearch/|' "$repo_file"
        sed -i 's|^baseurl=http://centos.mirrors.ovh.net/ftp.centos.org/7/updates/\$basearch/|baseurl=http://vault.centos.org/7.9.2009/updates/\$basearch/|' "$repo_file"
        sed -i 's|^baseurl=http://centos.mirrors.ovh.net/ftp.centos.org/7/extras/\$basearch/|baseurl=http://vault.centos.org/7.9.2009/extras/\$basearch/|' "$repo_file"
        sed -i 's|^baseurl=http://centos.mirrors.ovh.net/ftp.centos.org/7/centosplus/\$basearch/|baseurl=http://vault.centos.org/7.9.2009/centosplus/\$basearch/|' "$repo_file"
    fi
}

# Main execution
echo "Starting CentOS 7 EOL repository update..."

# Find and update all relevant .repo files
for repo_file in /etc/yum.repos.d/*.repo; do
    if [ -f "$repo_file" ]; then
        echo "Processing $repo_file..."
        update_repo_file "$repo_file"
        update_linode_repo "$repo_file"
        update_ovh_repo "$repo_file"
    fi
done

# Clean YUM cache
echo "Cleaning YUM cache..."
yum -q clean all

# Disable problematic repositories
echo "Disabling problematic repositories..."
yum-config-manager --disable centos-sclo-sclo &> /dev/null

# List updates to verify
echo "Verifying repository configuration..."
yum -q list updates

echo "Repository configuration for EOL CentOS 7 updated successfully."
echo "Note: Script has handled standard CentOS, Linode, and OVH mirrors."
echo "Reference: https://community.centminmod.com/threads/centos-7-end-of-life-eol-june-30-2024.25589/"