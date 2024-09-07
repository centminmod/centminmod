#!/bin/bash
##########################################################################
# CentOS 7 is now EOL as of June 30, 2024
# Disable centos-sclo-sclo REPO
# Switch CentOS 7 mirrorlist.centos.org to archive vault.centos.org
##########################################################################

# Function to update repository configuration files
update_repo_file() {
  local repo_file="$1"
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
    sed -i 's|^mirrorlist=http://mirrorlist.centos.org.*|#mirrorlist=http://mirrorlist.centos.org|' "$repo_file"
    sed -i 's|^#baseurl=http://mirror.centos.org.*|baseurl=http://vault.centos.org/centos/$releasever/os/$basearch|' "$repo_file"
  fi
}

# Find and update all relevant .repo files
for repo_file in /etc/yum.repos.d/*.repo; do
  update_repo_file "$repo_file"
done

# Clean YUM cache
yum -q clean all

# Disable problematic repositories
yum-config-manager --disable centos-sclo-sclo &> /dev/null

# List updates to verify
yum -q list updates

echo "Repository configuration for EOL CentOS 7 updated successfully."
echo "https://community.centminmod.com/threads/centos-7-end-of-life-eol-june-30-2024.25589/"
