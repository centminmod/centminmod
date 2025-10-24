# CENTMINMOD PERSISTENT CUSTOM CONFIGURATIONS OPTIONS FILE
# ==============================================================================
# IMPORTANT: This file serves as a comprehensive reference with ALL possible
# options, NOT what should be used in a production environment!
# ==============================================================================
#
# Official Documentation: https://centminmod.com
# Persistent Config Guide: https://centminmod.com/upgrade.html#persistent
# Installation Location: /etc/centminmod/custom_config.inc
#
# This file will override default settings in centmin.sh for:
#   - Software upgrades
#   - Initial installations
#   - Module installations
#   - Service configurations
#
# ==============================================================================
# QUICK START GUIDE
# ==============================================================================
#
# 1. Create the persistent config directory:
#    mkdir -p /etc/centminmod
#
# 2. Copy this file (or create your own):
#    cp example/custom_config.inc /etc/centminmod/custom_config.inc
#
# 3. Edit and uncomment ONLY the variables you want to override:
#    nano /etc/centminmod/custom_config.inc
#
# 4. Run centmin.sh - your customizations will be automatically loaded
#
# 5. Backup your custom_config.inc regularly!
#
# ==============================================================================
# IMPORTANT NOTES
# ==============================================================================
#
# - Variables set here persist across Centmin Mod updates
# - Boolean variables use 'y' (yes/enabled) or 'n' (no/disabled)
# - Version variables should use exact upstream version numbers
# - Path variables must use absolute paths
# - Only uncomment and modify variables you need to change
# - Comment your customizations for future reference
# - Test changes on non-production systems first
#
# ==============================================================================
# TOTAL VARIABLES DOCUMENTED: 541
# CATEGORIES: 53
# BRANCH: 141.00beta01
# LAST UPDATED: 2025-10-24
# ==============================================================================

################################################################################
# TABLE OF CONTENTS
################################################################################
#
# 1.  Core System Settings (17 variables)
# 2.  Email & Notification Settings (2 variables)
# 3.  Git & Update Control (4 variables)
# 4.  Time & Logging Settings (8 variables)
# 5.  Menu & Interface Settings (4 variables)
# 6.  Network & IPv6 Settings (5 variables)
# 7.  SSH & Firewall Settings (7 variables)
# 8.  Compiler & Optimization Settings (23 variables)
# 9.  GCC Devtoolset Settings (9 variables)
# 10. Nginx Compilation Settings (14 variables)
# 11. Nginx Dynamic Modules (24 variables)
# 12. Nginx Static Modules (59 variables)
# 13. Nginx HTTP/2 & QUIC Settings (5 variables)
# 14. Nginx Operational Settings (24 variables)
# 15. Nginx Vhost Control (2 variables)
# 16. OpenResty Modules (36 variables)
# 17. Lua/LuaJIT Settings (5 variables)
# 18. SSL/TLS OpenSSL Settings (21 variables)
# 19. SSL/TLS Alternative Libraries (9 variables)
# 20. SSL/TLS Protocol Settings (7 variables)
# 21. Cloudflare Integration (2 variables)
# 22. PHP Compilation Settings (22 variables)
# 23. PHP PGO & Performance (12 variables)
# 24. PHP Extensions - Core (11 variables)
# 25. PHP Extensions - Cache/Performance (14 variables)
# 26. PHP Extensions - Compression (6 variables)
# 27. PHP Extensions - Databases/Data (9 variables)
# 28. PHP Extensions - Images (5 variables)
# 29. PHP Extensions - Other (21 variables)
# 30. MariaDB/MySQL Installation (14 variables)
# 31. PostgreSQL Settings (1 variable)
# 32. Redis Server Settings (2 variables)
# 33. Memcached Server Settings (7 variables)
# 34. ImageMagick Settings (5 variables)
# 35. Service Disable Flags (4 variables)
# 36. Repository Settings (8 variables)
# 37. NSD DNS Server (2 variables)
# 38. NTP Time Settings (1 variable)
# 39. Mail Services (2 variables)
# 40. Python Settings (4 variables)
# 41. Siege Benchmark Tool (2 variables)
# 42. Download & Mirror Settings (15 variables)
# 43. GeoIP & Maxmind Settings (2 variables)
# 44. TCP Settings (2 variables)
# 45. WordPress Settings (4 variables)
# 46. Let's Encrypt Settings (1 variable)
# 47. Menu Trigger Settings (6 variables)
# 48. Compression Tools (2 variables)
# 49. Performance Tools (4 variables)
# 50. Security Tools (1 variable)
# 51. MOTD & Alert Settings (1 variable)
# 52. Override Flags (2 variables)
# 53. Miscellaneous Settings (63 variables)
#
################################################################################

################################################################################
# 1. CORE SYSTEM SETTINGS
################################################################################

# Server notification email address (enter only 1 address)
# Used for: system alerts, CSF firewall notifications, disk alerts
# Git History: First added 2014-12-25 (d20a8601), Last modified 2016-02-15 (7240145c)
#EMAIL=''  

# Pushover.net push email notifications to mobile & tablets
# Requires: Pushover.net account signup
# Git History: First added 2014-12-25 (d20a8601), Last modified 2016-02-15 (7240145c)
#PUSHOVER_EMAIL=''  

# Set Timezone
# Examples: 'America/Los_Angeles', 'Europe/London', 'Asia/Tokyo'
# Default: Etc/UTC
# Git History: Added 2014-04-24 (c4b6d98a)
#ZONEINFO=Etc/UTC

# Text editor choice for command shortcuts
# Options: 'nano' or 'vim'
# Git History: Added 2014-06-08 (071dbb05)
#USEEDITOR='nano'

# Custom Nginx server name in HTTP headers
# Set to 'y' to use custom server string, 'n' for default 'nginx'
# Git History: Added 2014-04-24 (c4b6d98a)
#CUSTOMSERVERNAME='y'

# Custom Nginx server string value
# Only used if CUSTOMSERVERNAME='y'
# Git History: Added 2014-04-24 (c4b6d98a)
#CUSTOMSERVERSTRING='nginx centminmod'

# PHP-FPM configuration directory
# Default: '/usr/local/nginx/conf/phpfpmd'
# Git History: Added 2014-04-24 (c4b6d98a)
#PHPFPMCONFDIR='/usr/local/nginx/conf/phpfpmd'

# Unattended installation mode
# IMPORTANT: Keep at 'y' for best compatibility
# Git History: Added 2014-04-24 (c4b6d98a)
#UNATTENDED='y'

# Centmin Mod version check on startup
# Set to 'y' to check for updates, 'n' to skip
# Git History: Added 2014-06-06 (1f2e1c10)
#CMVERSION_CHECK='n'

# Perform yum check on centmin.sh exit
# Set to 'y' to check for system updates when exiting menu
# Git History: Added 2019-03-15 (c123209b)
#MENUEXIT_ALWAYS_YUMCHECK='y'  

# Centmin Mod debug mode
# Set to 'y' to enable verbose debug output, 'n' for normal operation
# Git History: First added 2018-05-02 (38371de2), Last modified 2018-05-02 (5f0ab4b5)
#CMSDEBUG='n'  

# CentOS Alpha/Beta testing support
# Set to 'y' to allow installation on alpha/beta OS releases
# Git History: Added 2024-07-06 (aba2c6f4)
#CENTOS_ALPHATEST='y'

# Tested CentOS version
# Latest tested OS version
# Git History: Added 2014-04-24 (c4b6d98a)
#TESTEDCENTOSVER='10.0'

# Tmpfs RAM disk size limit (in KB)
# Systems with >4GB RAM will use tmpfs for /svr-setup
# Default: 4000000 (4GB minimum)
# Git History: Added 2014-04-24 (d962e8b9)
#TMPFSLIMIT=4000000

# Temporary directory for installations
# Default: '/svr-setup'
# Git History: First added 2014-04-24 (c4b6d98a), Last modified 2014-04-24 (d962e8b9)
#DIR_TMP='/svr-setup'

# Centmin Mod log directory
# All installation and upgrade logs stored here
# Git History: Added 2020-08-28 (2054e5d4)
#CENTMINLOGDIR='/root/centminlogs'

# Configuration scan base directory
# Default: '/etc/centminmod'
# Git History: Added 2014-04-24 (c4b6d98a)
#CONFIGSCANBASE='/etc/centminmod'

# PHP configuration scan directory
# PHP .ini files stored here
# Git History: Added 2014-04-24 (c4b6d98a)
#CONFIGSCANDIR='/etc/centminmod/php.d'

################################################################################
# 2. EMAIL & NOTIFICATION SETTINGS
################################################################################

# Server notification email (critical alerts)
# Used for: CSF firewall, disk space alerts, installation notices
# Git History: First added 2014-12-25 (d20a8601), Last modified 2016-02-15 (7240145c)
#EMAIL=''  

# Pushover push notifications
# Mobile/tablet push notifications via Pushover.net
# Git History: First added 2014-12-25 (d20a8601), Last modified 2016-02-15 (7240145c)
#PUSHOVER_EMAIL=''  

################################################################################
# 3. GIT & UPDATE CONTROL
################################################################################

# Centmin Mod Git Repository URL
# Primary: https://github.com/centminmod/centminmod.git
# Backup:  https://gitlab.com/centminmod/centminmod.git
# Git History: First added 2015-06-18 (0e5b9a69), Last modified 2016-02-15 (7240145c)
#CMGIT='https://github.com/centminmod/centminmod.git'

# Auto Git Update
# Enable automatic silent background updates of Centmin Mod code
# Note: Use persistent config file to retain local customizations
# Git History: First added 2016-02-08 (84e9771a), Last modified 2016-02-15 (7240145c)
#AUTO_GITUPDATE='n'

# Local Centmin Mod Mirror
# Primary mirror for Centmin Mod components
# Git History: Added 2016-10-05 (f837022e)
#LOCALCENTMINMOD_MIRROR='https://parts.centminmod.com'

# GeoLite2 Database Domain
# Maxmind GeoLite2 database mirror
# Git History: Added 2024-03-25 (093360db)
#LOCAL_GEOLITE_TWO_DOMAIN='https://mxmind.centminmod.com'

################################################################################
# 4. TIME & LOGGING SETTINGS
################################################################################

# Timestamp install output
# Set to 'y' to add timestamps to all installation output
# Git History: Added 2015-06-14 (8ddcc29f)
#TS_INSTALL='n'

# Timestamp Nginx compilation
# Set to 'y' to display timing information during Nginx builds
# Git History: Added 2018-02-09 (67d73b94)
#TIME_NGINX='n'

# Timestamp PHP configuration and compilation
# Set to 'y' to display timing information during PHP builds
# Git History: Added 2018-02-09 (67d73b94)
#TIME_PHPCONFIGURE='n'

# Timestamp Memcached installation
# Set to 'y' to display timing information during Memcached install
# Git History: Added 2018-02-09 (67d73b94)
#TIME_MEMCACHED='n'

# Timestamp ImageMagick installation
# Set to 'y' to display timing information during ImageMagick install
# Git History: Added 2018-02-09 (67d73b94)
#TIME_IMAGEMAGICK='n'

# Timestamp Redis installation
# Set to 'y' to display timing information during Redis install
# Git History: Added 2018-02-09 (67d73b94)
#TIME_REDIS='n'

# Zstd compressed log rotation for Nginx
# Initial install only - compress Nginx logs with zstd
# Reference: https://community.centminmod.com/threads/16371/
# Git History: Added 2019-01-01 (6a58b0e9)
#ZSTD_LOGROTATE_NGINX='n'  

# Zstd compressed log rotation for PHP-FPM
# Initial install only - compress PHP-FPM logs with zstd
# Git History: Added 2019-01-01 (6a58b0e9)
#ZSTD_LOGROTATE_PHPFPM='n'  

################################################################################
# 5. MENU & INTERFACE SETTINGS
################################################################################

# Enable or disable menu mode
# Set to 'n' to disable interactive menu (command-line mode only)
# Git History: Added 2014-04-24 (c4b6d98a)
#ENABLE_MENU='y'

# Enable centmin.sh menu option 21 (Data Management Menu)
# Data backup, transfer, and management tools
# Git History: Added 2024-07-01 (ffe396a1)
#DATA_MENU_ENABLE='y'

# Perform yum check when exiting centmin.sh menu
# Check for system updates on menu exit
# Git History: Added 2019-03-15 (c123209b)
#MENUEXIT_ALWAYS_YUMCHECK='y'  

# Enable MySQL Admin Shell (menu option 6)
# Database and user management submenu
# Git History: Added 2024-07-04 (7a26a327)
#MYSQLADMIN_SHELL='y'

# Enable Persistent Config Menu (menu option 7)
# Advanced configuration management submenu
# Note: Must also set in custom_config.inc to activate menu option
# Git History: Not found in centmin.sh or example/custom_config.inc
#PERSISTENT_CONFIG_MENU='n'

################################################################################
# 6. NETWORK & IPv6 SETTINGS
################################################################################

# Force IPv4 for curl/wget commands
# Set to 'y' to force all downloads to use IPv4
# Git History: Added 2018-07-14 (99aae3ee)
#FORCE_IPVFOUR='y'  

# Disable system IPv6 support
# Set to 'y' to disable IPv6 at system level
# Reference: https://wiki.centos.org/FAQ/CentOS7#head-8984faf811faccca74c7bcdd74de7467f2fcd8ee
# Git History: Added 2017-05-24 (832254ec)
#DISABLE_IPVSIX='n'

# Disable IPv6 checks for Nginx vhosts
# When 'y', skip IPv6 support detection and vhost IPv6 configuration
# Git History: Added 2022-11-14 (eec812e6)
#VPS_IPSIX_CHECK_DISABLE='y'

# Debug mode for IPv6 checks
# Enable verbose IPv6 detection debugging
# Git History: Added 2022-11-14 (eec812e6)
#VPS_IPSIX_CHECK_DISABLE_DEBUG='n'

# Local GeoIP server version
# v3 or v4 API for GeoIP lookups
#VPS_GEOIPCHECK_V3='n'
#VPS_GEOIPCHECK_V4='y'

################################################################################
# 7. SSH & FIREWALL SETTINGS
################################################################################

# Re-enable root SSH logins on EL9+ systems
# Creates /etc/ssh/sshd_config.d/01-permitrootlogin.conf during install
# EL9 disables root SSH by default
# Git History: Added 2022-09-04 (15fd7bef)
#SSHD_REENABLE_ROOT_LOGINS='y'

# CSF Firewall PORTFLOOD override
# Set to 'y' to override default CSF PORTFLOOD values
# Only works if user hasn't manually customized PORTFLOOD settings
# Reference: https://community.centminmod.com/threads/14708/
# Git History: Added 2018-05-04 (ad3228d6)
#CSFPORTFLOOD_OVERRIDE='n'

# PORTFLOOD hit count
# Maximum hits allowed (max value: 20)
# Git History: Added 2018-05-04 (ad3228d6)
#PORTFLOOD_COUNT=20

# PORTFLOOD interval
# Time window in seconds for hit count (default: 300 = 5 minutes)
# Git History: Added 2018-05-04 (ad3228d6)
#PORTFLOOD_INTERVAL=300

# Firewalld disable
# Set to 'y' to disable firewalld service (uses iptables/CSF instead)
# Git History: Added 2014-07-10 (e24482eb)
#FIREWALLD_DISABLE='y'

# Maxmind License Key
# Your Maxmind account API key for GeoLite2 database downloads
# Override default with your own key
# Reference: https://community.centminmod.com/posts/80656/
# Git History: First added 2020-01-02 (2f339d20), Last modified 2023-10-28 (28b8c2de)
#MM_LICENSE_KEY="$GET_CMM_MM_LICENSE_KEY"

# Maxmind CSF source
# Use Maxmind GeoIP data for CSF firewall
# Git History: Added 2020-01-20 (50774ee9)
#MM_CSF_SRC='n'

################################################################################
# 8. COMPILER & OPTIMIZATION SETTINGS (GENERAL)
################################################################################

# CPU Architecture Target
# For Intel 64bit: set march=native for CPU-specific optimizations
# For generic builds: uses x86-64 or detected x86-64-v2/v3/v4 level
# Git History: Added 2017-09-09 (e8c33c03)
#MARCH_TARGETNATIVE='y'  

# Always use native march target
# Force march=native even on VPS (overrides smart VPS detection)
# Git History: Added 2018-12-02 (b46d28fb)
#MARCH_TARGETNATIVE_ALWAYS='n'  

# Clang Compiler for Nginx
# Set to 'y' to build Nginx with Clang instead of GCC
# Git History: First added 2015-06-09 (4c8cb410), Last modified 2016-07-09 (0163b0e6)
#CLANG='n'  

# Clang LTO for Nginx
# Enable Link Time Optimization when using Clang
# Git History: Added 2024-09-10 (340f492b)
#CLANG_LTO_ENABLE='y'  

# Clang Full LTO
# Use full Clang LTO linker instead of LTO Thin linker
# Git History: Added 2024-09-10 (acf07492)
#CLANG_LTO_FULL='n'  

# Clang 4.0+ Support
# Enable Clang 4.0+ optional support
# Reference: https://community.centminmod.com/threads/13729/
# Git History: Added 2017-01-31 (773c4265)
#CLANG_FOUR='n'  

# Clang 5.0+ Support
# Enable Clang 5.0+ optional support
# Git History: First added 2017-09-09 (f7905166), Last modified 2017-09-09 (9c05d6ba)
#CLANG_FIVE='n'  

# Clang 6.0+ Support
# Enable Clang 6.0+ optional support
# Git History: Added 2018-01-22 (4ed14e45)
#CLANG_SIX='n'  

# Clang for PHP-FPM
# Set to 'y' to build PHP-FPM with Clang compiler
# Git History: First added 2015-06-11 (975153ca), Last modified 2016-02-15 (7240145c)
#CLANG_PHP='n'  

# Clang for APC Cache
# Set to 'y' to build APC with Clang compiler
# Git History: First added 2015-06-15 (5eb54b07), Last modified 2016-02-15 (7240145c)
#CLANG_APC='n'  

# Clang for Memcached
# Set to 'y' to build Memcached (menu option 10) with Clang
# Git History: First added 2015-06-15 (5eb54b07), Last modified 2016-02-15 (7240145c)
#CLANG_MEMCACHED='n'  

# GCC Intel CPU Optimizations for PHP-FPM
# Enable GCC compiler with Intel CPU optimizations for PHP-FPM
# Git History: First added 2015-12-28 (665ec555), Last modified 2018-06-23 (e14e142b)
#GCCINTEL_PHP='y'  

# GCC Optimization Level
# Choices: -O2, -O3, or -Ofast (only for GCC, not Clang)
# -O3 provides best balance of performance and compilation time
# Git History: Added 2016-07-09 (8e4956d9)
#GCC_OPTLEVEL='-O3'

# GCC Function Alignment
# Set -falign-functions=32 for GCC compiles of Nginx, PHP-FPM, pigz
# Reference: https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html
# Git History: Added 2019-11-09 (1b041cfd)
#GCC_FALIGN_FUNCTION='n'

# Intel Optimizations
# Experimental Intel-compiled optimizations for auto-detected Intel CPUs
# Git History: Added 2014-08-04 (43bfcb19)
#INTELOPT='n'

# Parallel Download Mode
# Use subshells to download tarballs in parallel for faster installs
# Git History: Added 2015-12-22 (0c6a7114)
#PARALLEL_MODE=y

# ccache Installation
# Install ccache compiler cache to speed up recompiles
# Git History: Added 2014-04-24 (c4b6d98a)
#CCACHEINSTALL='y'

# ccache YUM Install
# Install ccache via YUM instead of source compilation
# Git History: Added 2025-07-15 (087e9061)
#CCACHE_YUM_INSTALL='y'

# ccache Disable CPP2
# Disable second-pass cpp for ccache
#CCACHE_CPP2_DISABLE='n'

# ccache Version
# Version to install if compiling from source
# Git History: Added 2015-06-09 (4c8cb410)
#CCACHE_VER="3.7.12"

# ccache Cache Size
# Maximum cache size (e.g., '2.5G', '5G', '10G')
# Git History: Added 2014-04-24 (c4b6d98a)
#CCACHESIZE='2.5G'

# Make Threads
# Parallel make jobs (-j flag)
# Auto-calculated based on CPU count
# Git History: First added 2014-04-24 (c4b6d98a), Last modified 2025-10-19 (7584b0cd)
#MAKETHREADS=" -j$CPUS"

# LTO CPU Count
# Number of CPUs to use for Link Time Optimization
# Git History: Added 2024-06-28 (9b254ee1)
#CPUS_LTO=$(grep -c "processor" /proc/cpuinfo)

################################################################################
# 9. GCC DEVTOOLSET SETTINGS
################################################################################
#
# GCC Compiler Selection via devtoolset/gcc-toolset
#
# IMPORTANT: Only enable ONE devtoolset at a time!
#
# Auto-selected defaults by OS version:
#   EL7: devtoolset-10 (GCC 10)
#   EL8 <8.7: gcc-toolset-11 (GCC 11)
#   EL8 >=8.7: gcc-toolset-12 (GCC 12)
#   EL8 >=8.9: gcc-toolset-13 (GCC 13)
#   EL9 <9.1: System GCC 11
#   EL9 >=9.1: gcc-toolset-12 (GCC 12)
#   EL9 >=9.3: gcc-toolset-13 (GCC 13)
#   EL10: gcc-toolset-14 (GCC 14)
#
# Override these only if you need specific GCC versions
################################################################################

# Devtoolset-6 (GCC 6.2)
# EL7 only - legacy support
# Git History: Added 2017-02-23 (f0ae27f4)
#DEVTOOLSETSIX='n'  

# Devtoolset-7 (GCC 7.1)
# EL7 only
# Git History: First added 2017-09-09 (9c05d6ba), Last modified 2025-05-18 (395dea9c)
#DEVTOOLSETSEVEN='n'  

# Devtoolset-8 (GCC 8)
# EL6/EL7 support
# Git History: First added 2018-01-14 (d39b1448), Last modified 2025-05-18 (395dea9c)
#DEVTOOLSETEIGHT='n'  

# Devtoolset-9 (GCC 9)
# EL7 support
# Git History: First added 2020-02-20 (d9872bff), Last modified 2025-05-18 (395dea9c)
#DEVTOOLSETNINE='n'

# Devtoolset-10 (GCC 10)
# EL7 default
# Git History: First added 2021-11-06 (10eab12d), Last modified 2025-05-18 (395dea9c)
#DEVTOOLSETTEN='y'

# GCC Toolset 11 (GCC 11)
# EL8 <8.7 default, EL9 system default
# Git History: First added 2022-06-12 (a9a30fdf), Last modified 2025-05-18 (395dea9c)
#DEVTOOLSETELEVEN='n'

# GCC Toolset 12 (GCC 12)
# EL8 >=8.7 default, EL9 >=9.1 default
# Also used for PHP PGO fallback (PHP_PGO_FALLBACK_GCC='y')
# Git History: First added 2023-11-06 (65793ad3), Last modified 2025-05-18 (395dea9c)
#DEVTOOLSETTWELVE='n'

# GCC Toolset 13 (GCC 13)
# EL8 >=8.9 default, EL9 >=9.3 default
# Git History: First added 2023-11-06 (65793ad3), Last modified 2025-05-18 (395dea9c)
#DEVTOOLSETTHIRTEEN='y'

# GCC Toolset 14 (GCC 14)
# EL10 default
# Git History: First added 2024-09-12 (6e9f4564), Last modified 2025-05-18 (395dea9c)
#DEVTOOLSETFOURTEEN='y'

# GCC Toolset 15 (GCC 15)
# Future support
# Git History: First added 2024-09-20 (2244b83f), Last modified 2025-05-18 (395dea9c)
#DEVTOOLSETFIFTTEEN='n'

# Use devtoolset for PHP compilation
# Enable devtoolset GCC for GCCINTEL_PHP='y' builds
# Git History: Added 2018-06-23 (e14e142b)
#DEVTOOLSET_PHP='n'  

# Use devtoolset for Nginx compilation
# Enable devtoolset even on EL7 Nginx compiles
# Git History: Added 2016-10-05 (da1d0614)
#NGINX_DEVTOOLSETGCC='y'  

# Use devtoolset generally
# Enable devtoolset wherever possible/coded
# Git History: Added 2016-10-05 (cd458b18)
#GENERAL_DEVTOOLSETGCC='n'  

# Use devtoolset for crypto libraries
# Enable devtoolset for LibreSSL/OpenSSL compiles
# Git History: First added 2017-01-19 (91701974), Last modified 2018-09-19 (7f0176c4)
#CRYPTO_DEVTOOLSETGCC='y'  

################################################################################
# 10. NGINX COMPILATION SETTINGS
################################################################################

# Nginx Version
# Nginx version to compile and install
# Check: https://nginx.org/en/download.html
# Git History: First added 2014-04-24 (c4b6d98a), Last modified 2024-02-17 (5d801902)
#NGINX_VERSION='1.29.2'  

# Freenginx Installation
# Use Maxim Dounin's Freenginx fork instead of official Nginx
# Reference: https://freenginx.org/en/download.html
# Git History: Added 2024-02-17 (5d801902)
#FREENGINX_INSTALL='n'

# Freenginx Version
# Freenginx version if FREENGINX_INSTALL='y'
# Git History: Added 2024-02-17 (5d801902)
#FREENGINX_VERSION='1.29.2'

# Freenginx Backport Patches
# Backport Freenginux fixes to official Nginx builds
# Git History: Added 2024-07-11 (f23e90d4)
#FREENGINX_BACKPORT_PATCHES='n'

# Nginx Angie Version
# Angie is another Nginx fork (not currently used)
# Git History: Added 2024-03-27 (95eba223)
#NGINX_ANGIE_VERSION='Angie-1.10.1'

# Nginx gsplit-dwarf Optimization
# Reduce object file sizes during compilation
# Reference: https://community.centminmod.com/posts/44072/
# Git History: Added 2017-01-31 (616146e2)
#NGX_GSPLITDWARF='y'  

# Nginx ld.gold Linker
# Use ld.gold linker instead of default ld
# Options: -fuse-ld=bfd or -fuse-ld=gold
# Reference: https://community.centminmod.com/posts/44037/
# Git History: Added 2017-02-01 (d478ecd6)
#NGX_LDGOLD='y'  

# Nginx Mold Linker
# Use experimental mold linker for faster linking
# Reference: https://github.com/rui314/mold
# Git History: Added 2022-01-24 (5820ffcf)
#NGX_LDMOLD='n'  

# Mold Linker Version
# Mold RPM version for EL7
# Git History: Added 2022-01-24 (5820ffcf)
#MOLD_VERSION='1.3.0'  

# Mold Linker Version for EL8+
# Mold RPM version for EL8/EL9/EL10
#MOLD_VERSION_EL8='1.11.0'

# LTO Compression Control
# Control LTO compression levels on EL8/EL9 systems
# Git History: Added 2024-06-08 (b2e8eeb8)
#FLTO_COMP='y'  

# Nginx Fat LTO Objects
# Enable -ffat-lto-objects flag (much slower compile times)
# Git History: Added 2019-08-03 (53316760)
#NGINX_FATLTO_OBJECTS='n'  

# Nginx No-Fat LTO Objects
# Enable -fno-fat-lto-objects flag (much slower compile times)
# Git History: Added 2019-08-05 (eee7cd03)
#NGINX_NOFATLTO_OBJECTS='n'  

# Nginx OpenSSL Fat LTO Objects
# Enable -ffat-lto-objects for Nginx OpenSSL builds
# Recommended: keep at 'n'
# Git History: Added 2019-08-03 (53316760)
#NGINXOPENSSL_FATLTO_OBJECTS='n'  

# Nginx OpenSSL No-Fat LTO Objects
# Enable -fno-fat-lto-objects for Nginx OpenSSL builds
# Recommended: keep at 'n'
# Git History: Added 2019-08-05 (eee7cd03)
#NGINXOPENSSL_NOFATLTO_OBJECTS='n'  

# Nginx Format Security Flags
# Compile Nginx with -Wformat -Werror=format-security flags
# Git History: Added 2017-11-20 (dbdc47a7)
#NGINXCOMPILE_FORMATSEC='y'  

# Nginx Secured Compilation
# Apply more secure compilation options for Nginx
# Git History: Added 2023-11-30 (bb4166df)
#NGINX_SECURED='y'  

# Nginx Symbolic Functions & As-Needed
# Enable -Wl,-Bsymbolic-functions -Wl,--as-needed linker flags
# Git History: Added 2024-09-11 (5ecc12f3)
#NGINX_SYMBOLIC_AS_NEEDED='y'  

# Nginx PIE Compilation
# Build Nginx with Position-Independent code (PIC) / Position-Independent Executables (PIEs)
# Git History: Added 2018-06-09 (7297bcd6)
#NGINXCOMPILE_PIE='y'  

# Strip Nginx Binary
# Set to 'y' to strip Nginx binary to reduce file size
# Git History: First added 2015-05-08 (85f955b2), Last modified 2016-02-15 (7240145c)
#STRIPNGINX='y'  

################################################################################
# 11. NGINX DYNAMIC MODULES
################################################################################
#
# Dynamic modules are loaded at runtime via load_module directive
# Located in: /usr/local/nginx/modules/
# Loaded in: /usr/local/nginx/conf/dynamic-modules.conf
################################################################################

# Manual Override for Dynamic Modules
# Set to 'y' to manually manage nginx dynamic modules directory
# Git History: Added 2017-04-08 (1f42886e)
#NGXDYNAMIC_MANUALOVERRIDE='n'  

# NJS (nginScript) Dynamic Module
# JavaScript scripting for Nginx
# Reference: https://nginx.org/en/docs/njs/
# Git History: First added 2016-03-30 (e4e0cdff), Last modified 2016-08-18 (3d65b58d)
#NGXDYNAMIC_NJS='n'

# XSLT Dynamic Module
# XSLT transformation module
# Git History: Added 2016-03-20 (743823cd)
#NGXDYNAMIC_XSLT='n'

# Perl Dynamic Module
# Embedded Perl in Nginx
# Git History: First added 2016-03-11 (86eb20be), Last modified 2016-03-20 (743823cd)
#NGXDYNAMIC_PERL='n'

# Image Filter Dynamic Module
# On-the-fly image transformation
# Git History: First added 2016-02-11 (c1917491), Last modified 2016-02-15 (7240145c)
#NGXDYNAMIC_IMAGEFILTER='y'

# GeoIP Legacy Dynamic Module
# GeoIP legacy module
# Git History: First added 2016-02-11 (c1917491), Last modified 2016-03-20 (bbfcbe5f)
#NGXDYNAMIC_GEOIP='n'

# GeoIP2 Lite Dynamic Module
# MaxMind GeoIP2 Lite module
# Reference: https://github.com/leev/ngx_http_geoip2_module
# Git History: Added 2018-05-09 (a20e729e)
#NGXDYNAMIC_GEOIPTWOLITE='n'

# Stream Dynamic Module
# TCP/UDP load balancing and proxying
# Git History: First added 2016-02-11 (c1917491), Last modified 2016-03-20 (bbfcbe5f)
#NGXDYNAMIC_STREAM='n'

# Stream GeoIP Dynamic Module
# GeoIP for stream module (Nginx 1.11.3+)
# Git History: First added 2016-07-13 (522abef2), Last modified 2016-08-18 (3d65b58d)
#NGXDYNAMIC_STREAMGEOIP='n'  

# Stream RealIP Dynamic Module
# Real client IP for stream module (Nginx 1.11.4+)
# Git History: Added 2016-09-11 (866f3697)
#NGXDYNAMIC_STREAMREALIP='n'  

# Headers More Dynamic Module
# Set, add, and clear request/response headers
# Reference: https://github.com/openresty/headers-more-nginx-module
# Git History: First added 2016-02-11 (c1917491), Last modified 2016-03-20 (bbfcbe5f)
#NGXDYNAMIC_HEADERSMORE='y'

# Set-Misc Dynamic Module
# Various set_xxx directives
# Reference: https://github.com/openresty/set-misc-nginx-module
# Git History: First added 2016-02-11 (c1917491), Last modified 2016-03-20 (bbfcbe5f)
#NGXDYNAMIC_SETMISC='y'

# Echo Dynamic Module
# Echo module for debugging
# Reference: https://github.com/openresty/echo-nginx-module
# Git History: First added 2016-02-11 (c1917491), Last modified 2016-03-20 (bbfcbe5f)
#NGXDYNAMIC_ECHO='y'

# Lua Dynamic Module
# Lua scripting for Nginx
# Reference: https://github.com/openresty/lua-nginx-module
# Git History: First added 2016-02-11 (c1917491), Last modified 2016-03-20 (bbfcbe5f)
#NGXDYNAMIC_LUA='y'

# SrcCache Dynamic Module
# Transparent subrequest-based caching
# Git History: First added 2016-05-20 (17e56883), Last modified 2016-08-18 (3d65b58d)
#NGXDYNAMIC_SRCCACHE='n'

# Devel Kit Dynamic Module
# Nginx development kit (required for other modules)
# Git History: First added 2016-05-20 (17e56883), Last modified 2016-08-18 (3d65b58d)
#NGXDYNAMIC_DEVELKIT='y'

# Memc Dynamic Module
# Memcached module
# Git History: First added 2016-05-20 (17e56883), Last modified 2016-08-18 (3d65b58d)
#NGXDYNAMIC_MEMC='n'

# Redis2 Dynamic Module
# Redis module
# Git History: First added 2016-05-20 (17e56883), Last modified 2016-08-18 (3d65b58d)
#NGXDYNAMIC_REDISTWO='n'

# ngx_pagespeed Dynamic Module
# Google PageSpeed module
# Git History: First added 2016-02-17 (d6a95b4e), Last modified 2016-02-19 (b6c17ed2)
#NGXDYNAMIC_NGXPAGESPEED='n'

# Brotli Dynamic Module
# Brotli compression module
# Reference: https://github.com/google/ngx_brotli
# Git History: First added 2016-02-19 (53ebfe64), Last modified 2019-02-04 (938d7b8b)
#NGXDYNAMIC_BROTLI='y'

# Zstd Dynamic Module
# Zstandard compression module
# Reference: https://github.com/tokers/zstd-nginx-module
# Git History: Added 2024-06-20 (9c1acda3)
#NGXDYNAMIC_ZSTD='n'

# Fancy Index Dynamic Module
# Enhanced directory listing
# Reference: https://github.com/aperezdc/ngx-fancyindex
# Git History: First added 2016-06-14 (f12a240f), Last modified 2016-08-18 (3d65b58d)
#NGXDYNAMIC_FANCYINDEX='y'

# Hide Length Dynamic Module
# Hide Content-Length header
# Git History: Added 2016-08-18 (3d65b58d)
#NGXDYNAMIC_HIDELENGTH='y'

# Test Cookie Dynamic Module
# Bot detection via test cookie
# Git History: Added 2017-11-10 (a501a85c)
#NGXDYNAMIC_TESTCOOKIE='n'

# VHost Stats Dynamic Module
# Virtual host statistics
# Reference: https://github.com/vozlt/nginx-module-vts
# Git History: Added 2017-09-22 (d00d540b)
#NGXDYNAMIC_VHOSTSTATS='n'

################################################################################
# 12. NGINX STATIC MODULES
################################################################################
#
# Static modules are compiled directly into the Nginx binary
################################################################################

# Nginx RealIP Module
# Get real client IP behind proxies
# Reference: http://nginx.org/en/docs/http/ngx_http_realip_module.html
# Git History: Added 2017-04-05 (f62fd944)
#NGINX_REALIP='y'  

# Nginx RDNS Module
# Reverse DNS lookups
# Reference: https://github.com/flant/nginx-http-rdns
# Git History: First added 2016-01-28 (2c8c8968), Last modified 2016-02-15 (7240145c)
#NGINX_RDNS='n'  

# Nginx NJS (nginScript)
# JavaScript scripting for Nginx (static compilation)
# Reference: https://www.nginx.com/blog/
# Git History: First added 2015-09-24 (737a30dd), Last modified 2016-02-15 (7240145c)
#NGINX_NJS='n'  

# Nginx NJS Version
# nginScript version tag
# Reference: https://github.com/nginx/njs
# Git History: Added 2023-05-31 (769907e5)
#NGINX_NJS_VER='0.9.0'  

# Nginx GeoIP Legacy Module
# MaxMind GeoIP legacy databases
# Git History: First added 2014-04-24 (d962e8b9), Last modified 2016-02-15 (7240145c)
#NGINX_GEOIP='y'  

# Nginx GeoIP Memory Caching
# Cache GeoIP databases in memory (faster) vs disk
# Git History: First added 2015-11-29 (54038e98), Last modified 2016-02-15 (7240145c)
#NGINX_GEOIPMEM='y'  

# Nginx GeoIP2 Lite Module
# MaxMind GeoIP2 Lite databases (static)
# Reference: https://github.com/leev/ngx_http_geoip2_module
# Git History: Added 2018-05-09 (a20e729e)
#NGINX_GEOIPTWOLITE='n'  

# Nginx SPDY Support
# Google SPDY protocol (deprecated, use HTTP/2)
# Git History: First added 2014-04-24 (c4b6d98a), Last modified 2016-02-15 (7240145c)
#NGINX_SPDY='n'  

# Nginx SPDY Patched
# Cloudflare HTTP/2 + SPDY patch
# Reference: https://github.com/cloudflare/sslconfig/blob/master/patches/nginx__http2_spdy.patch
# Git History: Added 2016-06-22 (f265c8c3)
#NGINX_SPDYPATCHED='n'  

# Nginx Stub Status Module
# Basic status page (/nginx_status)
# Required for Nginx statistics
# Reference: http://nginx.org/en/docs/http/ngx_http_stub_status_module.html
# Git History: First added 2015-09-23 (60487368), Last modified 2016-02-15 (7240145c)
#NGINX_STUBSTATUS='y'  

# Nginx Substitution Module
# Text substitution filter
# Reference: http://nginx.org/en/docs/http/ngx_http_sub_module.html
# Git History: First added 2015-09-23 (60487368), Last modified 2016-02-15 (7240145c)
#NGINX_SUB='y'  

# Nginx Addition Module
# Add content before/after response
# Reference: http://nginx.org/en/docs/http/ngx_http_addition_module.html
# Git History: First added 2015-09-23 (60487368), Last modified 2016-02-15 (7240145c)
#NGINX_ADDITION='y'  

# Nginx Image Filter Module
# On-the-fly image transformations (resize, rotate, crop)
# Reference: http://nginx.org/en/docs/http/ngx_http_image_filter_module.html
# Git History: First added 2015-09-23 (60487368), Last modified 2016-02-15 (7240145c)
#NGINX_IMAGEFILTER='y'  

# Nginx Perl Module
# Embedded Perl support
# Reference: http://nginx.org/en/docs/http/ngx_http_perl_module.html
# Git History: First added 2016-03-11 (86eb20be), Last modified 2016-03-20 (743823cd)
#NGINX_PERL='n'  

# Nginx XSLT Module
# XSLT transformations
# Reference: http://nginx.org/en/docs/http/ngx_http_xslt_module.html
# Git History: Added 2016-03-20 (743823cd)
#NGINX_XSLT='n'  

# Nginx Length Hiding Module
# Hide response length from clients
# Reference: https://github.com/nulab/nginx-length-hiding-filter-module
# Git History: Added 2016-08-18 (3d65b58d)
#NGINX_LENGTHHIDE='n'  

# Nginx Length Hide Git
# Pull from git master (only if NGINX_LENGTHHIDE='y')
# Git History: Added 2016-08-18 (3d65b58d)
#NGINX_LENGTHHIDEGIT='y'  

# Nginx Test Cookie Module
# Bot detection via test cookie
# Reference: https://github.com/kyprizel/testcookie-nginx-module
# Git History: Added 2017-11-10 (a501a85c)
#NGINX_TESTCOOKIE='n'  

# Nginx Test Cookie Git
# Pull from git (only if NGINX_TESTCOOKIE='y')
# Git History: Added 2017-11-10 (a501a85c)
#NGINX_TESTCOOKIEGIT='n'  

# Nginx Cache Purge Module
# Purge cached content on demand
# Reference: https://github.com/FRiCKLE/ngx_cache_purge/
# Git History: First added 2015-09-23 (60487368), Last modified 2016-02-15 (7240145c)
#NGINX_CACHEPURGE='y'  

# Nginx Cache Purge Version
# Git History: First added 2014-12-06 (ff4e5d72), Last modified 2016-02-15 (7240145c)
#NGINX_CACHEPURGEVER='2.5.3'

# Nginx Access Key Module
# Token-based access control
# Git History: First added 2015-09-23 (60487368), Last modified 2016-02-15 (7240145c)
#NGINX_ACCESSKEY='n'

# Nginx HTTP Concat Module
# Concatenate multiple files in one request
# Reference: https://github.com/alibaba/nginx-http-concat
# Git History: First added 2015-09-23 (60487368), Last modified 2016-02-15 (7240145c)
#NGINX_HTTPCONCAT='n'  

# Nginx Thread Pools
# Thread pools for blocking operations
# Reference: https://www.nginx.com/blog/thread-pools-boost-performance-9x/
# Git History: First added 2015-09-23 (60487368), Last modified 2016-02-15 (7240145c)
#NGINX_THREADS='y'  

# Nginx Slice Module
# Range requests for large files
# Reference: https://nginx.org/en/docs/http/ngx_http_slice_module.html
# Git History: First added 2018-09-01 (b48f0e53), Last modified 2018-09-01 (d493c2b6)
#NGINX_SLICE='n'  

# Nginx Stream Module
# TCP/UDP load balancing and proxying
# Reference: http://nginx.org/en/docs/stream/ngx_stream_core_module.html
# Git History: First added 2015-06-16 (27c4ca24), Last modified 2016-02-15 (7240145c)
#NGINX_STREAM='y'  

# Nginx Stream GeoIP
# GeoIP for stream module (Nginx 1.11.3+)
# Reference: http://hg.nginx.org/nginx/rev/558db057adaa
# Git History: First added 2016-07-13 (522abef2), Last modified 2016-07-27 (27a207e4)
#NGINX_STREAMGEOIP='y'  

# Nginx Stream RealIP
# Real IP for stream module (Nginx 1.11.4+)
# Reference: http://hg.nginx.org/nginx/rev/9cac11efb205
# Git History: Added 2016-09-11 (866f3697)
#NGINX_STREAMREALIP='y'  

# Nginx Stream SSL Preread
# SNI routing for stream module (Nginx 1.11.5+)
# Reference: https://nginx.org/en/docs/stream/ngx_stream_ssl_preread_module.html
# Git History: Added 2016-12-05 (a1df61d6)
#NGINX_STREAMSSLPREREAD='y'  

# Nginx RTMP Module
# RTMP streaming server
# Reference: https://github.com/arut/nginx-rtmp-module
# Git History: First added 2015-06-14 (3003b45d), Last modified 2018-09-01 (d493c2b6)
#NGINX_RTMP='n'  

# Nginx FLV Module
# FLV streaming
# Reference: http://nginx.org/en/docs/http/ngx_http_flv_module.html
# Git History: First added 2015-06-16 (e833a6f5), Last modified 2018-09-01 (d493c2b6)
#NGINX_FLV='n'  

# Nginx MP4 Module
# MP4 streaming with seeking
# Reference: http://nginx.org/en/docs/http/ngx_http_mp4_module.html
#NGINX_MP4='n'

# Nginx Auth Request Module
# Subrequest-based authentication
# Reference: http://nginx.org/en/docs/http/ngx_http_auth_request_module.html
# Git History: First added 2015-07-22 (ec079c56), Last modified 2016-02-15 (7240145c)
#NGINX_AUTHREQ='n'  

# Nginx Secure Link Module
# Secure links with expiration and hash
# Reference: http://nginx.org/en/docs/http/ngx_http_secure_link_module.html
# Git History: First added 2015-06-16 (e833a6f5), Last modified 2016-02-15 (7240145c)
#NGINX_SECURELINK='y'  

# Nginx Fancy Index Module
# Enhanced directory listings
# Reference: https://github.com/aperezdc/ngx-fancyindex
# Git History: First added 2015-06-16 (e833a6f5), Last modified 2016-02-15 (7240145c)
#NGINX_FANCYINDEX='y'  

# Nginx Fancy Index Version
# Git History: Added 2016-02-15 (e29f35ff)
#NGINX_FANCYINDEXVER='0.5.2'  

# Nginx VHost Stats Module
# Virtual host traffic statistics
# Reference: https://github.com/vozlt/nginx-module-vts
# Git History: First added 2015-05-29 (ec807dbd), Last modified 2016-02-15 (7240145c)
#NGINX_VHOSTSTATS='n'  

# Nginx Brotli Module (Static)
# Brotli compression support (static compilation)
# Reference: https://github.com/google/ngx_brotli
# Git History: First added 2015-10-14 (69e731a0), Last modified 2019-02-04 (938d7b8b)
#NGINX_LIBBROTLI='n'  

# Nginx Brotli Static Only
# Enable only pre-compressed brotli support, disable on-the-fly
# Git History: First added 2016-02-10 (a575dadd), Last modified 2016-02-15 (7240145c)
#NGINX_LIBBROTLISTATIC='n'  

# Nginx Brotli Dependency Update
# Experimental manual update of Google Brotli dependency
# Git History: First added 2018-12-31 (edf3e32b), Last modified 2019-02-04 (938d7b8b)
#NGINX_BROTLIDEP_UPDATE='n'  

# Nginx Brotli New Method
# Use ngx_brotli with Brotli 1.1.0+ dependency
# Git History: Added 2023-09-26 (c5f6d13d)
#NGINX_BROTLI_NEW_METHOD='y'  

# Nginx Zstd Module (Static)
# Zstandard compression support (static compilation)
# Reference: https://github.com/tokers/zstd-nginx-module
# Git History: Added 2024-06-20 (9c1acda3)
#NGINX_ZSTD='n'  

# Nginx PageSpeed Module
# Google PageSpeed optimization module
# Reference: https://www.modpagespeed.com/
# Git History: First added 2014-04-24 (c4b6d98a), Last modified 2016-02-15 (7240145c)
#NGINX_PAGESPEED='n'  

# Nginx PageSpeed Git Master
# Install from GitHub master instead of release version
# Git History: First added 2014-04-24 (c4b6d98a), Last modified 2016-02-15 (7240145c)
#NGINX_PAGESPEEDGITMASTER='n'  

# Nginx PageSpeed Version
# Git History: First added 2014-04-24 (c4b6d98a), Last modified 2016-02-15 (7240145c)
#NGXPGSPEED_VER='1.14.33.1-RC1'

# Nginx PageSpeed PSOL Version
# Git History: First added 2014-04-24 (c4b6d98a), Last modified 2016-02-15 (7240145c)
#NGINX_PAGESPEEDPSOL_VER='1.14.36.1'

# Nginx Passenger Module
# Phusion Passenger application server
# Requires: addons/passenger.sh run beforehand
# Git History: First added 2014-04-24 (d962e8b9), Last modified 2016-02-15 (7240145c)
#NGINX_PASSENGER='n'  

# Nginx WebDAV Module
# WebDAV protocol support
# Includes nginx-dav-ext-module
# Git History: First added 2014-04-24 (c4b6d98a), Last modified 2016-02-15 (7240145c)
#NGINX_WEBDAV='n'  

# Nginx WebDAV Ext Module Version
# Git History: First added 2014-04-24 (c4b6d98a), Last modified 2016-02-15 (7240145c)
#NGINX_EXTWEBDAVVER='0.0.3'  

# Nginx Libatomic Support
# Configure Nginx with libatomic support
# Git History: First added 2014-04-24 (c4b6d98a), Last modified 2016-02-15 (7240145c)
#NGINX_LIBATOMIC='y'  

# Nginx Libatomic Version
# Newer than system default 7.2 for CentOS 7 only
# Reference: https://github.com/ivmai/libatomic_ops/releases
# Git History: Added 2020-06-14 (8f5cb350)
#NGINX_LIBATOMIC_VERSION='7.6.12'  

# Nginx HTTP Redis Module
# Redis HTTP interface
# Reference: http://wiki.nginx.org/HttpRedisModule
# Git History: First added 2015-07-26 (b2c2fed5), Last modified 2016-02-15 (7240145c)
#NGINX_HTTPREDIS='y'  

# Nginx HTTP Redis Version
# Git History: First added 2015-07-26 (b2c2fed5), Last modified 2016-02-15 (7240145c)
#NGINX_HTTPREDISVER='0.4.0-cmm'  

# Nginx PCRE Support
# PCRE & PCRE-JIT support
# Git History: Added 2022-01-24 (5820ffcf)
#NGINX_PCRE='y'  

# Nginx PCRE JIT
# PCRE JIT compilation for faster regex
# Git History: First added 2014-04-24 (c4b6d98a), Last modified 2016-02-15 (7240145c)
#NGINX_PCREJIT='y'  

# Nginx PCRE Dynamic
# Compile Nginx PCRE as dynamic instead of static library
# Git History: Added 2018-08-18 (755991cf)
#NGINX_PCRE_DYNAMIC='y'  

# Nginx PCRE Version
# PCRE 8.x version
# Git History: First added 2014-04-24 (c4b6d98a), Last modified 2016-02-15 (7240145c)
#NGINX_PCREVER='8.45'  

# Nginx PCRE2 Support
# Optional PCRE2 for Nginx 1.21.5+
# Git History: First added 2021-12-29 (dbf5bac3), Last modified 2025-05-23 (1919df4e)
#NGINX_PCRE_TWO='n'  

# Nginx PCRE2 Version
# PCRE2 10.x version
# Git History: Added 2021-12-29 (dbf5bac3)
#NGINX_PCRETWOVER='10.39'  

# Nginx Custom zlib
# Use custom zlib instead of system version
# Git History: Added 2017-02-05 (171071d2)
#NGINX_ZLIBCUSTOM='y'  

# Nginx zlib Version
# Git History: Added 2017-02-05 (171071d2)
#NGINX_ZLIBVER='1.3.1'  

# Nginx Video Module Control
# When 'y', enables NGINX_SLICE, NGINX_RTMP, NGINX_FLV, NGINX_MP4
# Git History: Added 2018-09-01 (d493c2b6)
#NGINX_VIDEO='n'  

# Nginx ModSecurity Support
# ModSecurity WAF module
# Reference: https://github.com/SpiderLabs/ModSecurity
# Git History: First added 2016-01-18 (5de61813), Last modified 2016-02-15 (7240145c)
#NGINX_MODSECURITY='n'  

# Nginx ModSecurity JSON Logs
# Switch to JSON log format
# To revert: manually edit /usr/local/nginx/modsec/modsecurity.conf
# Git History: Added 2021-12-15 (bc5e3595)
#NGINX_MODSECURITY_JSONLOGS='n'  

# Nginx ModSecurity MaxMind
# Build ModSecurity with libmaxminddb (currently disabled due to compile issues)
# Git History: Added 2019-05-01 (6f009c43)
#NGINX_MODSECURITY_MAXMIND='y'  

# ModSecurity OWASP Ruleset Version
# OWASP Core Rule Set version
# Reference: https://github.com/coreruleset/coreruleset/releases
# Git History: Added 2017-08-05 (10863ad9)
#MODSECURITY_OWASPVER='4.4.0'

# Nginx TLS Fingerprint Module
# JA3 TLS fingerprinting
# Reference: https://github.com/centminmod/nginx-ssl-fingerprint
# Git History: Added 2021-12-15 (7a5b2e5a)
#NGINX_TLS_FINGERPRINT='n'  

################################################################################
# 13. NGINX HTTP/2 & QUIC SETTINGS
################################################################################

# Nginx HTTP/2 Support
# Enable HTTP/2 protocol
# Reference: https://community.centminmod.com/threads/4127/
#NGINX_HTTP2='y'

# Nginx HTTP/2 Server Push
# HTTP/2 server push feature
# Reference: https://community.centminmod.com/threads/11910/
# Git History: Added 2017-06-07 (5fa91cb6)
#NGINX_HTTPPUSH='n'  

# Nginx HTTP/2 HPACK Full Encoding
# Maximum allowed Nginx version for HPACK patch
# Git History: Added 2022-01-27 (6914d664)
#NGINX_HPACK_ALLOWED_VER='1024005'  

# Nginx Dynamic TLS Records
# Optimize TLS over TCP to reduce latency
# Maximum allowed Nginx version for Dynamic TLS patch
# Reference: https://blog.cloudflare.com/optimizing-tls-over-tcp-to-reduce-latency/
# Git History: Added 2016-06-11 (cfcc21e7)
#NGINX_DYNAMICTLS='n'  
# Git History: Added 2022-01-27 (6914d664)
#NGINX_DYNAMICTLS_ALLOWED_VER='1024005'  

# Nginx HTTP/3 QUIC Support
# Enable HTTP/3 QUIC protocol (experimental)
# Requires quicTLS OpenSSL fork
# Conflicts with AWS_LC_SWITCH
# Git History: Added 2024-06-20 (1cb19e7b)
#NGINX_QUIC_SUPPORT='n'

# Nginx kTLS Support
# Kernel TLS offload (requires OpenSSL 3.0+ and Linux Kernel 5.2+)
# Git History: Added 2022-01-24 (5820ffcf)
#NGINX_KTLS='n'  

# Nginx zlib-ng Support
# 64-bit OS only - Nginx compiled against zlib-ng
# Reference: https://github.com/Dead2/zlib-ng
# Git History: Added 2016-07-29 (bf22a708)
#NGINX_ZLIBNG='n'  

################################################################################
# 14. NGINX OPERATIONAL SETTINGS
################################################################################

# Install Nginx
# Set to 'n' to skip Nginx installation
# Git History: First added 2014-04-24 (c4b6d98a), Last modified 2024-02-17 (5d801902)
#NGINX_INSTALL='y'  

# Nginx Update Maintenance Mode
# Put nginx major version updates into 503 maintenance mode
# Reference: https://community.centminmod.com/posts/26485/
# Git History: First added 2016-02-14 (58e6bb59), Last modified 2016-02-15 (7240145c)
#NGINX_UPDATEMAINTENANCE='n'

# Nginx Upgrade Sleep Time
# Seconds to wait during Nginx upgrades
# Git History: First added 2014-04-24 (c4b6d98a), Last modified 2016-02-15 (7240145c)
#NGINXUPGRADESLEEP='3'

# Switch Nginx to systemd
# Use CentOS 7+ systemd service file for Nginx
# Git History: Added 2022-04-19 (b316d4a2)
#SWITCH_NGINX_SYSTEMD='y'  

# Auto-tune client_max_body_size
# Automatically tune client_max_body_size in nginx.conf
# Git History: Added 2017-10-10 (ab670f1e)
#AUTOTUNE_CLIENTMAXBODY='y'  

# Auto-tune Nginx Backlog
# On non-OpenVZ systems, override Nginx NGX_LISTEN_BACKLOG
# Modifies: src/os/unix/ngx_linux_config.h
# Git History: Added 2019-06-02 (ff439e51)
#AUTOHARDTUNE_NGINXBACKLOG='y'  

# Use Nginx Main Ext Log Format
# Use default 'combined' log format instead of custom 'main_ext'
# 'main_ext' format designed for Nginx Amplify compatibility
# Git History: Added 2017-05-15 (7be4f430)
#USE_NGINXMAINEXTLOGFORMAT='n'  

# Nginx Config Allow Override
# Allow Centmin Mod to update nginx.conf defaults when revised
# Git History: First added 2017-04-15 (18abfb26), Last modified 2019-08-08 (631b5a11)
#NGINX_ALLOWOVERRIDE='y'  

# Nginx SSL Cache Allow Override
# Dynamically tune nginx ssl_session_cache based on detected memory
# File: /usr/local/nginx/conf/ssl_include.conf
# Git History: Added 2019-08-08 (631b5a11)
#NGINX_SSLCACHE_ALLOWOVERRIDE='n'  

# Nginx Gzip Multi Status
# Enable HTTP 207 Multi-Status gzip compression (for Nextcloud, etc.)
# Reference: https://trac.nginx.org/nginx/ticket/394
# Git History: Added 2024-01-05 (fd3b9c91)
#NGINX_GZIP_MULTI_STATUS='n'  

# Nginx Patch Enable
# Set to 'y' to allow NGINXPATCH_DELAY before Nginx configure/patching
# Git History: First added 2014-04-24 (c4b6d98a), Last modified 2016-02-15 (7240145c)
#NGINXPATCH='y'  

# Nginx Patch Delay
# Seconds to pause before Nginx configure during upgrades
# Git History: First added 2014-07-07 (fe91aa3e), Last modified 2016-02-15 (7240145c)
#NGINXPATCH_DELAY='1'  

# Nginx DHParam Size
# Diffie-Hellman parameter size: 2048 or 3072
# Git History: Added 2022-03-29 (9323ba6d)
#NGINX_DHPARAM_SIZE='2048'  

# Nginx io_uring Patch
# Experimental AIO patch for Linux 5.1+ Kernel systems
# Git History: Added 2019-08-06 (cfbfb2e0)
#NGINX_IOURING_PATCH='n'  

# Nginx Debug Mode
# Enable and reinstall Nginx with debug logging
# References:
#   - https://nginx.org/en/docs/debugging_log.html
#   - https://wiki.nginx.org/Debugging
# Git History: First added 2015-09-14 (254cfd01), Last modified 2016-02-15 (7240145c)
#NGINX_DEBUG='n'  

# Nginx Module Alt Order
# Alternative nginx configure module ordering
# Git History: First added 2015-10-16 (c782ea54), Last modified 2016-02-15 (7240145c)
#NGXMODULE_ALTORDER='y'  

# Nginx Compile Export
# Export symbols when mixing static and dynamic compiled libraries
# Git History: Added 2018-08-18 (099af26f)
#NGINX_COMPILE_EXPORT='y'  

# Nginx Zero Downtime Reload
# Zero downtime reloading on nginx upgrades
# Git History: Added 2016-07-16 (0d517c5a)
#NGINX_ZERODT='n'  

# Nginx 1.23+ Compatibility
# Allow installation of Nginx 1.23+ versions
# Git History: Added 2022-06-22 (ad94d327)
#NGINX_ONETWOTHREE_COMPAT='y'  

# Nginx Max Error Byte Limit
# Modify NGX_MAX_ERROR_STR hardcoded 2048 limit
# Reference: http://openresty-reference.readthedocs.io/en/latest/Lua_Nginx_API/#print
# Git History: Added 2017-02-22 (1b94e288)
#NGINX_MAXERRBYTELIMIT='2048'  

# Nginx Backup
# Backup Nginx configuration before upgrades
# Git History: First added 2014-04-24 (c4b6d98a), Last modified 2016-02-15 (7240145c)
#NGINXBACKUP='y'

# Nginx OCSP Stapling Cache Override
# Override default OCSP stapling cache refresh time (3600 seconds)
# Git History: Added 2020-04-15 (598da664)
#NGINX_STAPLE_CACHE_OVERRIDE='n'  

# Nginx OCSP Stapling Cache TTL
# OCSP stapling cache refresh time in seconds
# Default: 86400 (24 hours)
# Git History: Added 2020-04-15 (598da664)
#NGINX_STAPLE_CACHE_TTL='86400'  

# Nginx Vhost SSL
# Enable prompt for self-signed SSL vhost in menu option 2
# Git History: First added 2015-06-21 (804a4a5d), Last modified 2016-02-15 (7240145c)
#NGINX_VHOSTSSL='y'  

# Nginx CPU Auto-tune (New Method)
# Revised worker_process auto-tuning for >12 CPU thread servers
# Git History: Added 2019-04-29 (eaf7f798)
#NGINXCPU_AUTOTUNE_NEW='y'  

# Nginx Pre-Static Include Files
# Add pre-staticfiles-local.conf & pre-staticfiles-global.conf includes
# Git History: Added 2017-07-02 (c151a51d)
#VHOST_PRESTATICINC='y'  

# Nginx Directory
# Nginx installation directory
# Git History: Added 2014-04-24 (c4b6d98a)
#NGINXDIR='/usr/local/nginx'

# Nginx Config Directory
# Nginx configuration directory
# Git History: Added 2014-04-24 (c4b6d98a)
#NGINXCONFDIR="${NGINXDIR}/conf"

# Nginx Backup Directory
# Nginx backup storage directory
# Git History: Added 2014-04-24 (c4b6d98a)
#NGINXBACKUPDIR='/usr/local/nginxbackup'

# Nginx Main Hostname Blank Index
# Set main hostname public web root index.html to blank
# Git History: Added 2022-06-05 (e5fead66)
#NGINX_MAINHOSTNAME_BLANK_INDEX='n'  

################################################################################
# 15. NGINX VHOST CONTROL
################################################################################

# CloudFlare Include Control
# Enable/disable cloudflare.conf include file in vhosts
# 'y' = uncommented (enabled), 'n' = commented out (disabled)
# Git History: Added 2018-04-25 (e1017a9f)
#VHOSTCTRL_CLOUDFLAREINC='n'

# Auto-Protect Include Control
# Enable/disable autoprotect-$vhostname.conf include file
# 'y' = uncommented (enabled), 'n' = commented out (disabled)
# Git History: Added 2018-04-25 (e1017a9f)
#VHOSTCTRL_AUTOPROTECTINC='y'

################################################################################
# 16. OPENRESTY MODULES
################################################################################

# OpenResty Nginx Modules
# Set to 'n' to disable all OpenResty modules
# Note: Some modules are required for Centmin Mod functionality
# Git History: First added 2014-04-24 (c4b6d98a), Last modified 2018-08-29 (c740ca2e)
#NGINX_OPENRESTY='y'  

# OpenResty Headers More Module
# Set/add/clear request and response headers
# Reference: https://github.com/openresty/headers-more-nginx-module
# Git History: First added 2015-09-23 (60487368), Last modified 2016-02-15 (7240145c)
#ORESTY_HEADERSMORE='y'  

# OpenResty Headers More Git
# Use git master instead of version-specific release
# Git History: First added 2016-02-14 (e4b0cd4f), Last modified 2016-02-15 (7240145c)
#ORESTY_HEADERSMOREGIT='n'  

# Nginx Headers More Version
# Git History: First added 2014-04-24 (d962e8b9), Last modified 2016-02-15 (7240145c)
#NGINX_HEADERSMORE='0.38'

# OpenResty Memc Module Version
# Memcached module
# Reference: https://github.com/openresty/memc-nginx-module
# Git History: First added 2014-07-13 (7266ade2), Last modified 2016-02-15 (7240145c)
#ORESTY_MEMCVER='0.20'  

# OpenResty SrcCache Module Version
# Subrequest cache module
# Reference: https://github.com/openresty/srcache-nginx-module
# Git History: First added 2014-07-13 (7266ade2), Last modified 2016-02-15 (7240145c)
#ORESTY_SRCCACHEVER='0.33'  

# OpenResty Devel Kit Version
# Nginx development kit
# Reference: https://github.com/vision5/ngx_devel_kit
# Git History: First added 2015-05-08 (b1006c5f), Last modified 2016-02-15 (7240145c)
#ORESTY_DEVELKITVER='0.3.2'  

# OpenResty Set-Misc Module
# Various set_xxx directives
# Reference: https://github.com/openresty/set-misc-nginx-module
# Git History: Added 2018-08-18 (6e447fef)
#ORESTY_SETMISC='y'  

# OpenResty Set-Misc Git
# Use git master instead of version-specific release
# Git History: First added 2016-02-14 (ff6fc4c9), Last modified 2016-02-15 (7240145c)
#ORESTY_SETMISCGIT='n'  

# OpenResty Set-Misc Version
# Git History: First added 2015-05-08 (b1006c5f), Last modified 2016-02-15 (7240145c)
#ORESTY_SETMISCVER='0.33'  

# OpenResty Echo Module
# Echo module for debugging
# Reference: https://github.com/openresty/echo-nginx-module
# Git History: First added 2016-02-14 (d3fbd1d4), Last modified 2016-02-15 (7240145c)
#ORESTY_ECHOGIT='n'  

# OpenResty Echo Version
# Git History: First added 2015-05-08 (6026a7a5), Last modified 2016-02-15 (7240145c)
#ORESTY_ECHOVER='0.63'  

# OpenResty Redis2 Version
# Redis module
# Reference: https://github.com/openresty/redis2-nginx-module
# Git History: First added 2015-07-26 (b2c2fed5), Last modified 2016-02-15 (7240145c)
#ORESTY_REDISVER='0.15'  

# Nginx Sticky Module
# Session affinity (sticky sessions)
# Reference: https://bitbucket.org/nginx-goodies/nginx-sticky-module-ng
# Git History: First added 2014-07-12 (418c8ef1), Last modified 2016-02-15 (7240145c)
#NGINX_STICKY='n'  

# Nginx Sticky Version
# Git History: First added 2014-07-12 (418c8ef1), Last modified 2016-02-15 (7240145c)
#NGINX_STICKYVER='master'

# Nginx Upstream Check Module
# Health checks for upstream servers
# Reference: https://github.com/yaoweibin/nginx_upstream_check_module
# Git History: First added 2014-07-12 (418c8ef1), Last modified 2016-02-15 (7240145c)
#NGINX_UPSTREAMCHECK='n'  

# Nginx Upstream Check Version
# Git History: First added 2014-07-12 (418c8ef1), Last modified 2016-02-15 (7240145c)
#NGINX_UPSTREAMCHECKVER='0.3.0'

# Additional OpenResty module versions
# Git History: First added 2015-05-25 (f56179dc), Last modified 2016-02-15 (7240145c)
#ORESTY_LUAMEMCACHEDVER='0.17'  
# Git History: First added 2015-05-25 (f56179dc), Last modified 2016-02-15 (7240145c)
#ORESTY_LUAMYSQLVER='0.28'  
# Git History: First added 2015-05-25 (f56179dc), Last modified 2016-02-15 (7240145c)
#ORESTY_LUAREDISVER='0.29'  
# Git History: First added 2015-05-25 (f56179dc), Last modified 2016-02-15 (7240145c)
#ORESTY_LUADNSVER='0.23'  
# Git History: First added 2015-05-25 (f56179dc), Last modified 2016-02-15 (7240145c)
#ORESTY_LUAUPLOADVER='0.11'  
# Git History: First added 2015-05-25 (f56179dc), Last modified 2016-02-15 (7240145c)
#ORESTY_LUAWEBSOCKETVER='0.13'  
# Git History: First added 2015-05-25 (f56179dc), Last modified 2016-02-15 (7240145c)
#ORESTY_LUALOCKVER='0.09'  
# Git History: First added 2015-05-25 (f56179dc), Last modified 2016-02-15 (7240145c)
#ORESTY_LUASTRINGVER='0.16'  
# Git History: First added 2015-05-25 (963385d2), Last modified 2016-02-15 (7240145c)
#ORESTY_LUAREDISPARSERVER='0.13'  
# Git History: First added 2015-05-25 (963385d2), Last modified 2016-02-15 (7240145c)
#ORESTY_LUAUPSTREAMCHECKVER='0.08'  
# Git History: First added 2015-05-25 (963385d2), Last modified 2016-02-15 (7240145c)
#ORESTY_LUALRUCACHEVER='0.15'  
# Git History: First added 2015-05-25 (963385d2), Last modified 2016-02-15 (7240145c)
#ORESTY_LUARESTYCOREVER='0.1.31'  
# Git History: Added 2018-10-30 (d91957ae)
#ORESTY_LUASTREAMVER='0.0.16'  
# Git History: First added 2015-05-25 (963385d2), Last modified 2016-02-15 (7240145c)
#ORESTY_LUAUPSTREAMVER='0.07'  
# Git History: First added 2015-05-25 (4098192a), Last modified 2016-02-15 (7240145c)
#ORESTY_LUALOGGERSOCKETVER='0.1'  
# Git History: First added 2015-05-25 (4098192a), Last modified 2016-02-15 (7240145c)
#ORESTY_LUACOOKIEVER='master'  
# Git History: First added 2015-05-25 (73a334aa), Last modified 2016-02-15 (7240145c)
#ORESTY_LUAUPSTREAMCACHEVER='0.1.1'  

################################################################################
# 17. LUA/LUAJIT SETTINGS
################################################################################

# LuaJIT Git Installation
# Install LuaJIT 2.1 from dev branch
# Reference: http://repo.or.cz/w/luajit-2.0.git/shortlog/refs/heads/v2.1
# Git History: First added 2015-05-28 (93b45a21), Last modified 2016-02-15 (7240145c)
#LUAJIT_GITINSTALL='y'  

# LuaJIT Git Install Version
# Branch version (v2.1) will override ORESTY_LUAGITVER if enabled
# Git History: First added 2015-05-28 (93b45a21), Last modified 2016-02-15 (7240145c)
#LUAJIT_GITINSTALLVER='2.1-agentzh'  

# OpenResty Lua Nginx Module
# Enable/disable Lua nginx modules
# Git History: First added 2015-05-25 (f56179dc), Last modified 2024-07-15 (3ff7980d)
#ORESTY_LUANGINX='n'  

# OpenResty Lua Nginx Module Version
# Reference: https://github.com/openresty/lua-nginx-module
# Git History: First added 2015-05-25 (f56179dc), Last modified 2016-02-15 (7240145c)
#ORESTY_LUANGINXVER='0.10.28'  

# OpenResty LuaJIT Version
# Reference: http://luajit.org/
# Git History: First added 2015-05-25 (f56179dc), Last modified 2016-02-15 (7240145c)
#ORESTY_LUAGITVER='2.0.5'  

# OpenResty Lua Stream Module
# Stream Lua nginx module
# Reference: https://github.com/openresty/stream-lua-nginx-module
# Git History: First added 2018-10-30 (d91957ae), Last modified 2024-07-15 (3ff7980d)
#ORESTY_LUASTREAM='y'  

# OpenResty Lua Stream Version
# Git History: Added 2018-10-30 (d91957ae)
#ORESTY_LUASTREAMVER='0.0.16'  

# Nginx Lua Stream Forced
# Force enable stream-lua-nginx for Nginx 1.17+
# Git History: Added 2019-05-22 (53ff6b12)
#NGX_LUASTREAM_FORCED='y'  

# Nginx Lua Upstream Module
# Lua upstream module
# Reference: https://github.com/openresty/lua-upstream-nginx-module
# Git History: Added 2016-03-20 (51508b36)
#NGX_LUAUPSTREAM='n'  

# Nginx Lua Upstream Cache Module
# Lua upstream cache module
# Reference: https://github.com/cloudflare/lua-upstream-cache-nginx-module
# Git History: Added 2016-03-20 (bbfcbe5f)
#NGX_LUAUPSTREAMCACHE='n'  

# Lua CJSON Version
# Fast JSON encoding/decoding library
# Reference: https://github.com/openresty/lua-cjson
# Git History: First added 2015-05-28 (e1355312), Last modified 2016-02-15 (7240145c)
#LUACJSONVER='2.1.0.9'  

################################################################################
# 18. SSL/TLS OPENSSL SETTINGS
################################################################################

# OpenSSL Source Compilation Disable
# Set to 'y' to disable OpenSSL source compile (use system OpenSSL)
# Git History: First added 2014-04-24 (c4b6d98a), Last modified 2016-02-15 (7240145c)
#NOSOURCEOPENSSL='y'  

# OpenSSL Version
# OpenSSL version for source compilation
# Reference: http://openssl.org/
# Git History: First added 2014-04-24 (c4b6d98a), Last modified 2018-11-21 (653bd21e)
#OPENSSL_VERSION='1.1.1w'  

# OpenSSL Version Fallback
# Fallback version if OPENSSL_VERSION uses 1.1.x branch
# Git History: Added 2017-01-06 (70a465c7)
#OPENSSL_VERSIONFALLBACK='1.1.1w'  

# OpenSSL Version Old Override
# Override if persistent config OPENSSL_VERSION is out of date
# Git History: Added 2018-11-21 (653bd21e)
#OPENSSL_VERSION_OLDOVERRIDE='1.1.1w'  

# OpenSSL QUIC Version
# quicTLS OpenSSL fork for HTTP/3 QUIC support
# Git History: Added 2022-11-02 (6e75b757)
#OPENSSL_QUIC_VERSION='OpenSSL_1_1_1w+quic'  

# OpenSSL Threading
# Enable threading support in OpenSSL 1.1 branch
# Git History: Added 2017-10-03 (b3cf6022)
#OPENSSL_THREADS='y'  

# OpenSSL TLS 1.3
# Enable TLS 1.3 protocol in OpenSSL 1.1.1 builds
# Git History: Added 2018-05-10 (1a6bad26)
#OPENSSL_TLSONETHREE='y'  

# OpenSSL Custom Path
# Custom directory for OpenSSL 1.0.2+ installations
# Git History: Added 2016-08-26 (a9a5b39e)
#OPENSSL_CUSTOMPATH='/opt/openssl'  

# OpenSSL System Use
# Use system OpenSSL instead of custom compiled version
# Auto-enabled on EL8/EL9/EL10
# Git History: First added 2024-03-15 (6cdaab91), Last modified 2025-05-18 (395dea9c)
#OPENSSL_SYSTEM_USE='n'

# Cloudflare OpenSSL ChaCha20 Patch
# Apply Cloudflare's ChaCha20 patch
# Reference: https://github.com/cloudflare/sslconfig
# Git History: First added 2014-07-07 (1bc1ef39), Last modified 2016-02-15 (7240145c)
#CLOUDFLARE_PATCHSSL='n'  

# OpenSSL ECDSA Patch
# ECDSA optimization patch
# Reference: https://community.centminmod.com/posts/57725/
# Git History: Added 2018-01-02 (16d900f3)
#OPENSSLECDSA_PATCH='n'  

# OpenSSL ECDHX Patch
# ECDH optimization patch
# Reference: https://community.centminmod.com/posts/57726/
# Git History: Added 2018-01-02 (16d900f3)
#OPENSSLECDHX_PATCH='n'  

# OpenSSL Equal Cipher Patch
# Equal preference cipher patch
# Reference: https://community.centminmod.com/posts/57916/
# Git History: Added 2018-01-06 (c635434d)
#OPENSSLEQUALCIPHER_PATCH='n'  

# Prioritize ChaCha20 in OpenSSL
# Prioritize ChaCha20 cipher in OpenSSL builds
# Reference: https://community.centminmod.com/threads/15708/
# Git History: Added 2018-10-01 (a54f5749)
#PRIORITIZE_CHACHA_OPENSSL='n'  

# Nginx Prioritize ChaCha20
# Prioritize ChaCha20 in Nginx SSL configuration
# Reference: https://community.centminmod.com/posts/67042/
# Git History: Added 2018-09-26 (bde188b1)
#NGINX_PRIORITIZECHACHA='n'  

# Cloudflare zlib
# Use Cloudflare optimized zlib fork
# Reference: https://blog.cloudflare.com/cloudflare-fights-cancer/
# Git History: First added 2017-12-13 (e8d813dd), Last modified 2018-06-11 (fbd55786)
#CLOUDFLARE_ZLIB='y'  

# Cloudflare zlib Dynamic
# Compile Nginx CF zlib as dynamic instead of static
# Git History: Added 2018-08-18 (099af26f)
#CLOUDFLARE_ZLIB_DYNAMIC='y'  

# Cloudflare zlib for OpenSSL
# Compile dynamically custom OpenSSL against Cloudflare zlib
# Git History: Added 2018-08-18 (04a5b166)
#CLOUDFLARE_ZLIB_OPENSSL='n'  

# Cloudflare zlib Reset
# If CLOUDFLARE_ZLIB='n', auto-revert gzip level from 9 to 5
# Git History: Added 2018-06-11 (fbd55786)
#CLOUDFLARE_ZLIBRESET='y'  

# Cloudflare zlib Auto Max
# Don't auto-raise nginx gzip level to 9 if using Cloudflare zlib
#CLOUDFLARE_ZLIBR AUTOMAX='n'

# Cloudflare zlib for PHP
# Use Cloudflare zlib for PHP-FPM instead of system zlib
# Git History: Added 2018-01-24 (cbc7ced3)
#CLOUDFLARE_ZLIBPHP='n'  

# Cloudflare zlib Debug
# Enable verbose debug mode during Cloudflare zlib installation
# Git History: Added 2017-12-15 (5ef9e7d1)
#CLOUDFLARE_ZLIBDEBUG='n'  

# Cloudflare zlib Version
# Git History: Added 2017-12-14 (577f71d8)
#CLOUDFLARE_ZLIBVER='1.3.3'

################################################################################
# 19. SSL/TLS ALTERNATIVE LIBRARIES
################################################################################

# LibreSSL Switch
# Use LibreSSL instead of OpenSSL for Nginx
# Overrides OPENSSL_SYSTEM_USE, disables Lua modules
# Git History: First added 2015-06-03 (a823dd20), Last modified 2017-05-10 (fdfc91e4)
#LIBRESSL_SWITCH='n'  

# LibreSSL Version
# LibreSSL version to compile
# Reference: http://www.libressl.org/
# Git History: First added 2015-06-03 (a823dd20), Last modified 2016-02-15 (7240145c)
#LIBRESSL_VERSION='3.9.2'  

# BoringSSL Switch
# Use BoringSSL instead of OpenSSL for Nginx (experimental)
# Overrides OPENSSL_SYSTEM_USE
# Git History: Added 2016-06-02 (f761a456)
#BORINGSSL_SWITCH='n'  

# BoringSSL Shared Library
# Build BoringSSL as shared library for dynamic Nginx compilation
# Git History: Added 2018-08-19 (8fa9a96c)
#BORINGSSL_SHARED='y'  

# BoringSSL Directory
# Git History: Added 2018-08-16 (617ce44a)
#BORINGSSL_DIR="/opt"

# AWS-LC Switch
# Use AWS-LC instead of OpenSSL for Nginx
# Overrides OPENSSL_SYSTEM_USE, conflicts with NGINX_QUIC_SUPPORT
# Only available on EL8/EL9/EL10
# Reference: https://github.com/aws/aws-lc
# Git History: First added 2024-06-10 (304dd425), Last modified 2024-06-20 (1cb19e7b)
#AWS_LC_SWITCH='n'

# AWS-LC Version
# AWS-LC version tag
# Reference: https://github.com/aws/aws-lc/tags
# Git History: Added 2024-06-21 (13e367fa)
#AWS_LC_VERSION='v1.60.0'

# AWS-LC Directory
# Git History: Added 2024-06-10 (304dd425)
#AWS_LC_DIR="/opt"

# AWS-LC Build Tests
# Run AWS-LC build tests during compilation
# Git History: Added 2024-06-10 (304dd425)
#AWS_LC_SWITCH_BUILD_TESTS='n'

################################################################################
# 20. SSL/TLS PROTOCOL SETTINGS
################################################################################

# SSL Protocol Modern
# Disable TLSv1.0 & TLSv1.1, support only TLSv1.2+ minimum
# Git History: Added 2021-12-18 (6d281e14)
#SSL_PROTOCOL_MODERN='y'  

# Disable TLS 1.0 Protocol
# Disable TLS 1.0 protocol (industry moving to deprecate for security)
# Git History: Added 2018-08-29 (52237520)
#DISABLE_TLSONEZERO_PROTOCOL='n'  

# Self-Signed SSL Certificate Settings
# Details for auto-generated self-signed SSL certificates
# Used in centmin.sh menu option 2 and 22
# Format: -subj "/C=${C}/ST=${ST}/L=${L}/O=${O}/OU=${OU}/CN=${vhostname}"

# Country Code (2 digits)
# Git History: First added 2015-09-02 (870e19b9), Last modified 2016-02-15 (7240145c)
#SELFSIGNEDSSL_C='US'

# State
# Git History: First added 2015-09-02 (870e19b9), Last modified 2016-02-15 (7240145c)
#SELFSIGNEDSSL_ST='California'

# Location (City)
# Git History: First added 2015-09-02 (870e19b9), Last modified 2016-02-15 (7240145c)
#SELFSIGNEDSSL_L='Los Angeles'

# Organization
# If blank, defaults to vhostname
# Git History: First added 2015-09-02 (870e19b9), Last modified 2016-02-15 (7240145c)
#SELFSIGNEDSSL_O=''

# Organizational Unit
# If blank, defaults to vhostname
# Git History: First added 2015-09-02 (870e19b9), Last modified 2016-02-15 (7240145c)
#SELFSIGNEDSSL_OU=''

################################################################################
# 21. CLOUDFLARE INTEGRATION
################################################################################

# Cloudflare Authenticated Origin Pull Certificate
# Setup guide: https://community.centminmod.com/threads/13847/
# Git History: Added 2018-01-27 (8c6f2ab2)
#CLOUDFLARE_AUTHORIGINPULLCERT='https://gist.githubusercontent.com/centminmod/020e3580eb03f1c36ced83b94fe4e1c5/raw/origin.crt'

# Vhost Cloudflare Authenticated Origin Pull
# Enable authenticated origin pull for new vhosts
# Git History: Added 2018-01-27 (8c6f2ab2)
#VHOST_CFAUTHORIGINPULL='y'

################################################################################
# 22. PHP COMPILATION SETTINGS
################################################################################

# PHP Version
# PHP version to compile and install
# Reference: https://www.php.net/
# Git History: First added 2014-04-24 (c4b6d98a), Last modified 2016-02-15 (7240145c)
#PHP_VERSION='8.3.21'  

# PHP Install
# Enable PHP-FPM installation
# Git History: First added 2014-04-24 (c4b6d98a), Last modified 2016-02-15 (7240145c)
#PHP_INSTALL='y'  

# PHP Mirror URL
# PHP source download mirror
# Git History: First added 2014-04-24 (c4b6d98a), Last modified 2016-02-15 (7240145c)
#PHP_MIRRORURL='https://www.php.net'

# PHP Upgrade Mirror URL
# Git History: First added 2014-04-24 (c4b6d98a), Last modified 2016-02-15 (7240145c)
#PHPUPGRADE_MIRRORURL="$PHP_MIRRORURL"

# PHP Secured Compilation
# Apply more secure compilation options for PHP-FPM
# Git History: Added 2023-11-30 (bb4166df)
#PHP_SECURED='y'  

# PHP PIE Compilation
# Build PHP-FPM with Position-Independent code/executables
# Git History: Added 2023-12-01 (0b24e133)
#PHP_SECURED_PIE='y'  

# PHP gsplit-dwarf
# Reduce object file sizes during PHP compilation
# Reference: https://community.centminmod.com/posts/44072/
# Git History: Added 2017-01-31 (616146e2)
#PHP_GSPLITDWARF='y'  

# PHP LTO
# Enable -flto compiler flag for GCC 4.8.5+ PHP-FPM compiles
# Currently not working with PHP 7.x
# Git History: First added 2017-06-26 (765fa429), Last modified 2017-09-09 (e8c33c03)
#PHP_LTO='n'  

# PHP Custom SSL
# Compile PHP-FPM against OpenSSL 1.0.2+ or LibreSSL 2.3+ (whichever Nginx uses)
# Git History: Added 2016-06-23 (bb3cb205)
#PHP_CUSTOMSSL='n'  

# PHP Custom SSL Force
# Force enable PHP_CUSTOMSSL on EL7 systems (requires additional work)
# Git History: Added 2022-09-09 (4bf3ce2c)
#PHP_CUSTOMSSL_FORCE='n'  

# PHP Patch
# Apply PHP patches if they exist
# Git History: Added 2018-12-09 (d00c75c8)
#PHP_PATCH='y'  

# PHP Patch OpenSSL 3.x Compatibility
# Compatibility patches for PHP 7.4 & 8.0 on EL9/EL10 with OpenSSL 3.x
# Git History: Added 2023-10-26 (be96b4fb)
#PHP_PATCH_OPENSSL_THREE='y'  

# PHP MySQL ND Patch Fix
# Backported mysqlnd patch for MariaDB compatibility (PHP <=7.2)
# Reference: https://community.centminmod.com/posts/86953/
# Git History: Added 2020-11-06 (1cb53780)
#PHP_MYSQLND_PATCH_FIX='y'  

# Strip PHP Binary
# Set to 'y' to strip PHP-FPM binary to reduce file size
# Git History: First added 2015-05-16 (20a9ee07), Last modified 2016-02-15 (7240145c)
#STRIPPHP='y'  

# PHP Make Test
# Set to 'y' to run 'make test' after PHP compilation (diagnostic purposes)
# Git History: First added 2014-06-06 (1f2e1c10), Last modified 2016-02-15 (7240145c)
#PHPMAKETEST=n  # set to y to enable make test after PHP make for diagnostic purposes

# PHP Debug Mode
# Enable --enable-debug PHP compile flag
# Git History: First added 2015-03-08 (cf6145d5), Last modified 2025-05-06 (406ff233)
#PHPDEBUGMODE='n'  

# PHP Clean Core Dumps
# Remove PHP-FPM core dumps when PHPDEBUGMODE='n'
# Git History: Added 2025-05-06 (406ff233)
#PHP_CLEAN_COREDUMPS='y'  

# PHP libgd External
# Use external libgd instead of bundled PHP gd version
# Auto-disabled on EL10 (system libgd 2.3 available)
# Git History: First added 2021-12-04 (6305b6ef), Last modified 2025-10-16 (5923fb87)
#PHP_LIBGD_EXTERNAL='y'  

# PHP GD AVIF Support
# Enable AVIF support in external libgd for PHP >8.1
# Git History: Added 2024-10-27 (b15f2ca9)
#PHP_GD_AVIF='y'  

# External libgd Version
# Reference: https://github.com/libgd/libgd/releases
# Git History: Added 2021-12-04 (6305b6ef)
#LIBGD_EXTERNAL_VER='2.3.3'  

# PHP libzip Support
# Use newer libzip instead of PHP embedded zip
# Git History: First added 2018-03-17 (d29b3c3c), Last modified 2018-11-21 (756aca5f)
#PHP_LIBZIP='n'  

# PHP Argon Support
# Alias for PHP_LIBZIP - enables libsodium and Argon2 support
# When PHP_ARGON='y', sets PHP_LIBZIP='y'
# Git History: Added 2018-11-21 (756aca5f)
#PHP_ARGON='y'  

# libzip Version
# Required for PHP 7.2+ with libsodium & Argon2
# Reference: https://github.com/nih-at/libzip/releases
# Git History: Added 2018-03-17 (d29b3c3c)
#LIBZIP_VER='1.11.4'  

# libsodium Version
# Modern crypto library
# Reference: https://github.com/jedisct1/libsodium/releases
# Git History: Added 2017-11-14 (4290ff8c)
#LIBSODIUM_VER='1.0.20'  

# libsodium Native Optimization
# Optimize for specific CPU (not portable between different CPUs)
# Git History: Added 2017-11-14 (4290ff8c)
#LIBSODIUM_NATIVE='n'  

# libargon2 Version
# Argon2 password hashing
# Reference: https://github.com/P-H-C/phc-winner-argon2
# Git History: Added 2017-11-14 (4290ff8c)
#LIBARGON_VER='20190702'  

# PHP Overwrite Config
# Show prompt to overwrite php-fpm.conf during PHP upgrades
#PHP_OVERWRITE CONF='y'

# Switch PHP-FPM to systemd
# Use CentOS 7+ systemd service file for PHP-FPM
# Reference: https://community.centminmod.com/threads/16511/
# Git History: Added 2019-03-11 (c1736664)
#SWITCH_PHPFPM_SYSTEMD='y'  

# PHP-FPM Install Directory
# Default directory for PHP-FPM INSTALL_ROOT routines
# Git History: Added 2022-06-06 (0c3e9e40)
#FPM_PHPFPM_INSTALLDIR='/home/installdir'

# PHP PID Path
# PHP-FPM PID file location (varies by OS version)
# EL8/9/10: /run/php-fpm/php-fpm.pid
# EL7: /var/run/php-fpm/php-fpm.pid
# Git History: First added 2022-08-30 (3a5d8ba0), Last modified 2025-05-18 (395dea9c)
#PHP_PID_PATH='/run/php-fpm/php-fpm.pid'
# Git History: First added 2022-09-22 (0a0654f9), Last modified 2025-05-18 (395dea9c)
#PHP_PID_PATHDIR='/run/php-fpm/'

# PHP MySQL Socket Path
# PHP configured --with-mysql-sock=${PHP_MYSQLSOCKPATH}/mysql.sock
# Git History: First added 2016-01-14 (20008b30), Last modified 2016-02-15 (7240145c)
#PHP_MYSQLSOCKPATH='/var/lib/mysql'

# PHP Mold Linker
# Use experimental mold linker for PHP-FPM linking
# Auto-enabled on EL10 if mold is available
# Git History: Added 2025-10-18 (2a545c44)
#PHP_LDMOLD='n'

# PHP Custom RPMs
# Use custom RPM packages for libsodium/libargon2 on EL10
# Auto-enabled on EL10, ~20-45 seconds faster than source compilation
# Falls back to source if RPM unavailable
# RPM mirror: https://parts.centminmod.com/centminmodparts/rpms/
# Git History: Added 2025-10-18 (42aa40f0)
#PHP_USE_CUSTOM_RPMS='n'

################################################################################
# 23. PHP PGO & PERFORMANCE SETTINGS
################################################################################

# PHP Profile Guided Optimization
# Enable PGO for 5-15% PHP performance improvement
# Reference: https://software.intel.com/en-us/blogs/2015/10/09/pgo-let-it-go-php
# Requires: 2+ CPU cores (or use PHP_PGO_ALWAYS='y')
# Compilation time: ~2-3x longer
# Git History: First added 2016-10-01 (8add4514), Last modified 2024-09-29 (43264a84)
#PHP_PGO='n'  

# PHP PGO Fallback to GCC 12
# Use GCC 12 instead of GCC 13+ for PHP PGO (workaround GCC 13+ bug)
# Git History: Added 2024-09-29 (43264a84)
#PHP_PGO_FALLBACK_GCC='y'  

# PHP PGO Fallback to GCC 11
# Ultra-conservative GCC 11 fallback for PHP <=8.1 PGO builds
# Overrides PHP_PGO_FALLBACK_GCC if enabled
#PHP_PGO_FALLBACK_GCC11='n'

# PHP PGO Always Enable
# Override PGO enable requirement for 1 CPU thread servers
# Git History: Added 2019-04-04 (b67d33ad)
#PHP_PGO_ALWAYS='n'  

# PHP PGO Training Runs
# Number of iterations during PGO PHP 7 training
# Higher values = better optimization, longer compilation
# Git History: Added 2018-12-02 (a65b2806)
#PHP_PGO_TRAINRUNS='10'  

# PHP PGO Append Label
# Append PGO label to PHP binary version string
# Git History: Added 2021-11-26 (d56040d1)
#PHP_PGO_APPEND_LABEL='y'  

# PHP PGO CentOS 6
# Use latest devtoolset for CentOS 6 PGO (needs GCC >4.4.7)
# Git History: Added 2016-10-01 (f30599a4)
#PHP_PGO_CENTOSSIX='n'  

# GCC Intel CPU Optimizations for PHP
# Enable PHP-FPM GCC compiler with Intel CPU optimizations
# Git History: First added 2015-12-28 (665ec555), Last modified 2018-06-23 (e14e142b)
#GCCINTEL_PHP='y'  

# PHP Tuning
# Auto-tune PHP-FPM configuration during initial install
# Git History: Added 2017-10-05 (63b0fffd)
#PHP_TUNING='n'  

# PHP Huge Pages
# Enable explicit huge pages support for PHP 7 on CentOS 7.x systems
# Git History: Added 2018-12-29 (fa21b383)
#PHP_HUGEPAGES='n'  

# PHP PCRE JIT Stack Size Adjust
# Enable raising PHP's default PCRE JIT stack size
# Reference: https://github.com/php/php-src/pull/2910
# Git History: Added 2019-10-27 (25e4b1ab)
#PHP_PCREJIT_STACKSIZE_ADJUST='n'  

# PHP PCRE JIT Stack Size
# Value to raise PCRE JIT stack size when PHP_PCREJIT_STACKSIZE_ADJUST='y'
# Used on centmin.sh menu option 5 runs
# Git History: Added 2019-10-27 (25e4b1ab)
#PHP_PCREJIT_STACKSIZE='512'  

# Auto-detect PHP Override
# When enabled, PHP updates always reinstall all extensions
# (even for minor PHP version updates)
# Git History: First added 2016-02-10 (b3cf7e2b), Last modified 2016-02-15 (7240145c)
#AUTODETECPHP_OVERRIDE='n'  

################################################################################
# 24. PHP EXTENSIONS - CORE
################################################################################

# PHP IMAP Extension
# IMAP email protocol support
# Git History: Added 2017-07-30 (3d3172e7)
#PHPIMAP='y'  

# PHP Fileinfo Extension
# Detect file MIME types
# Git History: First added 2014-04-24 (d962e8b9), Last modified 2016-02-15 (7240145c)
#PHPFINFO='n'  

# PHP Fileinfo Standalone
# Install Fileinfo as standalone module instead of compiled-in
# Git History: Added 2019-02-28 (fadc092f)
#PHPFINFO_STANDALONE='n'  

# PHP Process Control Extension
# PCNTL extension for process control
# Git History: First added 2014-04-24 (c4b6d98a), Last modified 2016-02-15 (7240145c)
#PHPPCNTL='y'  

# PHP Internationalization Extension
# intl extension for internationalization
# Git History: First added 2014-10-21 (bf44db3f), Last modified 2016-02-15 (7240145c)
#PHPINTL='y'  

# PHP Recode Extension
# Recode extension (deprecated)
# Git History: First added 2014-04-24 (d962e8b9), Last modified 2016-02-15 (7240145c)
#PHPRECODE=n  # Disable or Enable PHP Recode extension

# PHP SNMP Extension
# SNMP protocol support
# Git History: First added 2014-04-24 (d962e8b9), Last modified 2016-02-15 (7240145c)
#PHPSNMP='y'  

# PHP MSSQL Extension
# Microsoft SQL Server support
# Git History: Added 2016-07-01 (90a78784)
#PHPMSSQL='n'  

# PHP MSSQL Always Reinstall
# Always reinstall mssql extension on PHP recompiles
# Git History: First added 2016-07-05 (4be8b21f), Last modified 2017-09-23 (b6cb97d4)
#PHPMSSQL_ALWAYS='n'  

# PHP Embed SAPI
# Build PHP with embed SAPI library support (--enable-embed=shared)
# Git History: Added 2017-09-23 (b6cb97d4)
#PHPEMBED='y'  

# PHP FTP Extension
# FTP protocol support
# Git History: First added 2015-09-26 (32a26d4f), Last modified 2016-02-15 (7240145c)
#PHP_FTPEXT='y'  

################################################################################
# 25. PHP EXTENSIONS - CACHE/PERFORMANCE
################################################################################

# PHP Memcache Extension
# Memcache PHP extension (not Memcached server)
# Git History: First added 2015-09-26 (32a26d4f), Last modified 2016-02-15 (7240145c)
#PHP_MEMCACHE='y'  

# Memcache Extension Version
# Git History: First added 2014-04-24 (c4b6d98a), Last modified 2016-02-15 (7240145c)
#MEMCACHE_VERSION='3.0.8'  

# Memcache PHP 7 Compatible Version
# Git History: Added 2019-12-20 (00e46c06)
#MEMCACHE_COMPATVER='4.0.5.1'  

# PHP Memcached Extension
# Memcached PHP extension with advanced features
# Git History: First added 2015-09-26 (32a26d4f), Last modified 2016-02-15 (7240145c)
#PHP_MEMCACHED='y'  

# Memcached PHP Extension Version
# Git History: First added 2014-04-24 (c4b6d98a), Last modified 2016-02-15 (7240145c)
#MEMCACHEDPHP_VER='2.2.0'  

# Memcached PHP 7 Extension Version
# Git History: Added 2017-10-19 (1bf6e787)
#MEMCACHEDPHP_SEVENVER='3.1.5'  

# PHP Redis Extension
# Redis PHP extension
# Git History: First added 2015-04-07 (ff4e2d7a), Last modified 2016-02-15 (7240145c)
#PHPREDIS='y'  

# Redis PHP Extension Git
# Pull PHP 7 Redis extension from git instead of PECL
# Git History: Added 2016-06-21 (18ffdab3)
#REDISPHP_GIT='n'  

# Redis PHP Version (PHP <7.0)
# Git History: First added 2015-04-07 (ff4e2d7a), Last modified 2016-02-15 (7240145c)
#REDISPHP_VER='4.3.0'  

# Redis PHP 7.0-7.1 Version
# Git History: Added 2016-06-21 (18ffdab3)
#REDISPHPSEVEN_VER='5.3.7'  

# Redis PHP >=7.2 Version
# Git History: Added 2023-10-11 (38262abf)
#REDISPHPSEVENTWO_VER='6.1.0'  

# Zend OpCache Default
# Enable Zend OpCache by default
# Git History: First added 2014-04-24 (d962e8b9), Last modified 2016-02-15 (7240145c)
#ZOPCACHEDFT='y'

# Zend OpCache Version
# For PHP <=5.4
# Reference: https://pecl.php.net/package/ZendOpcache
# Git History: First added 2014-04-24 (c4b6d98a), Last modified 2016-02-15 (7240145c)
#ZOPCACHECACHE_VERSION='7.0.5'  

# Zend OpCache Override
# Override PHP 5.5, 5.6, 7.0 built-in Zend OpCache
# Git History: First added 2015-06-18 (d3eaece8), Last modified 2016-02-15 (7240145c)
#ZOPCACHE_OVERRIDE='n'  

# Xcache Version
# Xcache opcode cache (deprecated)
# Git History: First added 2014-04-24 (c4b6d98a), Last modified 2016-02-15 (7240145c)
#XCACHE_VERSION='3.2.0'  

# APC Cache Version
# Alternative PHP Cache (for older PHP versions)
# Git History: First added 2014-04-24 (c4b6d98a), Last modified 2016-02-15 (7240145c)
#APCCACHE_VERSION='3.1.13'  

# igbinary Extension
# Binary serialization for APC and Memcached
# Git History: First added 2015-09-26 (32a26d4f), Last modified 2016-02-15 (7240145c)
#IGBINARY_INSTALL='y'  

# igbinary Git Installation
# Install igbinary from git
# Git History: First added 2014-04-24 (c4b6d98a), Last modified 2016-02-15 (7240145c)
#IGBINARYGIT='y'

# igbinary Version
# Git History: First added 2014-04-24 (c4b6d98a), Last modified 2016-02-15 (7240145c)
#IGBINARY_VERSION='1.2.1'

################################################################################
# 26. PHP EXTENSIONS - COMPRESSION
################################################################################

# PHP Zopfli Extension
# Zopfli compression extension
# Reference: https://github.com/kjdev/php-ext-zopfli
# Git History: Added 2018-07-16 (d137253f)
#PHPZOPFLI='n'  

# PHP Zopfli Always Reinstall
# Always reinstall zopfli on PHP recompiles
# Git History: Added 2018-07-16 (d137253f)
#PHPZOPFLI_ALWAYS='n'  

# PHP Brotli Extension
# Brotli compression extension
# Reference: https://github.com/kjdev/php-ext-brotli
# Git History: Added 2019-02-10 (d55b8b24)
#PHP_BROTLI='n'  

# PHP LZ4 Extension
# LZ4 compression extension
# Reference: https://github.com/kjdev/php-ext-lz4
# Git History: Added 2019-02-10 (d55b8b24)
#PHP_LZFOUR='n'  

# PHP LZF Extension
# LZF compression extension
# Reference: https://github.com/php/pecl-file_formats-lzf
# Git History: Added 2019-02-10 (d55b8b24)
#PHP_LZF='n'  

# PHP Zstd Extension
# Zstandard compression extension
# Reference: https://github.com/kjdev/php-ext-zstd
# Git History: Added 2019-02-10 (d55b8b24)
#PHP_ZSTD='n'  

################################################################################
# 27. PHP EXTENSIONS - DATABASES/DATA
################################################################################

# PHP MongoDB Extension
# MongoDB database driver
# Git History: First added 2015-05-12 (aded548d), Last modified 2016-02-15 (7240145c)
#PHPMONGODB='n'  

# MongoDB PHP Extension Version
# Git History: First added 2015-05-12 (aded548d), Last modified 2016-02-15 (7240145c)
#MONGODBPHP_VER='1.7.5'  

# MongoDB PHP <=7.0 Max Version
# Git History: Added 2021-12-27 (51649e9d)
#MONGODBPHP_SEVEN_ZERO_VER='1.9.2'  

# MongoDB PHP 7.1+ Version
# Git History: Added 2021-12-27 (51649e9d)
#MONGODBPHP_SEVEN_VER='1.11.1'  

# MongoDB PHP 8.x Version
# Git History: Added 2021-12-27 (51649e9d)
#MONGODBPHP_EIGHT_VER='1.13.0'

# MongoDB PHP 8.2+ Version
# Git History: Added 2022-06-11 (059c2212)
#MONGODBPHP_EIGHTTWO_VER='1.20.0'  

# MongoDB SASL Support
# SASL authentication (not working yet, keep = n)
# Git History: First added 2015-05-12 (aded548d), Last modified 2016-02-15 (7240145c)
#MONGODB_SASL='n'  

# PDO PostgreSQL Extension Version
# PostgreSQL version for pdo-pgsql extension
# Git History: First added 2016-01-12 (903fad47), Last modified 2016-02-15 (7240145c)
#PDOPGSQL_PHPVER='11'  

################################################################################
# 28. PHP EXTENSIONS - IMAGES
################################################################################

# PHP Imagick Extension
# ImageMagick PHP extension
# Git History: First added 2015-09-20 (9131d2ed), Last modified 2016-02-15 (7240145c)
#PHPIMAGICK='y'  

# PHP Imagick Always Reinstall
# Always reinstall Imagick on PHP recompiles
# Git History: First added 2016-10-14 (195479e7), Last modified 2017-09-23 (b6cb97d4)
#PHPIMAGICK_ALWAYS='y'  

# Imagick PHP Extension Version
# Git History: First added 2014-04-24 (c4b6d98a), Last modified 2016-02-15 (7240145c)
#IMAGICKPHP_VER='3.4.4'  

# Imagick PHP 7 Extension Version
# Git History: First added 2021-12-27 (51649e9d), Last modified 2022-02-04 (3415397a)
#IMAGICKPHP_SEVEN_VER='3.7.0'  

# ImageMagick 7 Use REMI libzip
# If IMAGEMAGICK_SEVEN enabled, use REMI libzip
# Git History: Added 2024-10-31 (8e4f75f4)
#PHP_IMAGEMAGICK_SEVEN_USE_REMI_LIBZIP='y'  

################################################################################
# 29. PHP EXTENSIONS - OTHER
################################################################################

# PHP Mailparse Extension
# Email message parsing
# Git History: First added 2016-01-04 (805b5277), Last modified 2016-02-15 (7240145c)
#PHPMAILPARSE='y'  

# Mailparse PHP Extension Version
# Git History: First added 2016-01-04 (805b5277), Last modified 2016-02-15 (7240145c)
#MAILPARSEPHP_VER='2.1.6'  

# Mailparse PHP 7.0-7.3 Compatible Version
# Git History: First added 2016-01-12 (5681857d), Last modified 2016-02-15 (7240145c)
#MAILPARSEPHP_COMPATVER='3.1.3'  

# Mailparse PHP 7.4+ Compatible Version
# Git History: Added 2023-10-11 (38262abf)
#MAILPARSEPHPSEVENFOUR_COMPATVER='3.1.8'  

# PHP GeoIP Extension Always Reinstall
# GeoIP extension always reinstalled on PHP recompiles
# Git History: First added 2015-09-19 (a852e830), Last modified 2016-02-15 (7240145c)
#PHPGEOIP_ALWAYS='y'  

# PHP IonCube Loader
# IonCube encoded file support
# Install via: addons/ioncube.sh
# Git History: First added 2016-02-27 (f201c8bc), Last modified 2016-07-01 (90a78784)
#PHPIONCUBE='n'  

# PHP Timezone Database Extension
# Updated timezone database
# Reference: https://pecl.php.net/package/timezonedb
# Git History: Added 2019-11-09 (f6622b64)
#PHPTIMEZONEDB='y'  

# PHP Timezone DB Version
# Git History: Added 2019-11-09 (f6622b64)
#PHPTIMEZONEDB_VER='2025.1'  

# PHP Swoole Extension
# Async networking framework
# Reference: https://pecl.php.net/package/swoole
# Git History: Added 2021-11-27 (94745ca4)
#PHPSWOOLE='n'  

# PHP Swoole Version
# Git History: Added 2021-11-27 (94745ca4)
#PHPSWOOLE_VER='5.1.6'

# PHP Swoole 8.0 Version
# Git History: Added 2025-01-12 (f79acd1e)
#PHPSWOOLE_EIGHT_ZERO_VER='5.1.6'

# PHP Swoole 8.1 Version
# Git History: Added 2025-01-12 (f79acd1e)
#PHPSWOOLE_EIGHT_ONE_VER='6.0.0'

# PHP Swoole 8.2 Version
# Git History: Added 2025-01-12 (f79acd1e)
#PHPSWOOLE_EIGHT_TWO_VER='6.0.0'

# PHP Swoole 8.3 Version
# Git History: Added 2025-01-12 (f79acd1e)
#PHPSWOOLE_EIGHT_THREE_VER='6.0.0'

# PHP Swoole 8.4 Version
# Git History: Added 2025-01-12 (f79acd1e)
#PHPSWOOLE_EIGHT_FOUR_VER='6.0.0'

# PHP Swoole PHP 5 Max Version
# Git History: Added 2021-12-27 (51649e9d)
#PHPSWOOLE_FIVE_VER='2.0.5'  

# PHP Swoole PHP 7.0 Max Version
# Git History: Added 2021-12-27 (51649e9d)
#PHPSWOOLE_SEVEN_ZERO_VER='4.3.5'  

# PHP Swoole PHP 7.1 Max Version
# Git History: Added 2021-12-27 (51649e9d)
#PHPSWOOLE_SEVEN_ONE_VER='4.5.11'  

# PHP mcrypt PECL Extension
# PHP 7.2 deprecated mcrypt, this adds it back as PECL extension
# Git History: Added 2017-12-01 (de9238d7)
#PHP_MCRYPTPECL='y'  

# PHP mcrypt PECL Version
# Reference: https://pecl.php.net/package/mcrypt
# Git History: Added 2017-12-01 (de9238d7)
#PHP_MCRYPTPECLVER='1.0.9'  

# FFMPEG PHP Extension Version
# FFMPEG PHP extension (deprecated)
# Git History: First added 2014-04-24 (c4b6d98a), Last modified 2016-02-15 (7240145c)
#FFMPEGVER='0.6.0'

# Suhosin PHP Extension Version
# Suhosin security extension
# Git History: First added 2014-04-24 (c4b6d98a), Last modified 2016-02-15 (7240145c)
#SUHOSINVER='0.9.38'

################################################################################
# 30. MARIADB/MYSQL INSTALLATION SETTINGS
################################################################################

# Default MySQL Character Set
# Options: 'utf8' or 'utf8mb4'
# Only applies during initial Centmin Mod install
# Override via /etc/centminmod/custom_config.inc before initial install
# Git History: Added 2019-07-16 (fbaf57c8)
#SET_DEFAULT_MYSQLCHARSET='utf8'

# MariaDB Install via RPM
# Install MariaDB via RPM packages (not recommended for <256MB RAM)
# Git History: Added 2014-04-24 (c4b6d98a)
#MDB_INSTALL='n'

# MariaDB YUM Repo Install
# Install MariaDB via CentOS YUM repository (recommended method)
# Git History: Added 2014-04-24 (c4b6d98a)
#MDB_YUMREPOINSTALL='y'

# MariaDB Update Maintenance Mode
# Put MariaDB major version updates into 503 maintenance mode
# Reference: https://community.centminmod.com/posts/26485/
# Git History: First added 2016-02-14 (58e6bb59), Last modified 2016-05-21 (e72ba489)
#MARIADB_UPDATEMAINTENANCE='n'

# MariaDB Jemalloc
# Switch from system glibc to jemalloc memory allocator for MariaDB 10.4+
# Git History: Added 2015-06-28 (4badc690)
#MARIADB_JEMALLOC='n'

# MariaDB 10.2 Install
# Git History: First added 2019-03-28 (5ef98ad6), Last modified 2025-05-18 (395dea9c)
#MARIADB_INSTALLTENTWO='n'  

# MariaDB 10.3 Install
# Git History: First added 2018-05-29 (849cd133), Last modified 2025-05-18 (395dea9c)
#MARIADB_INSTALLTENTHREE='n'  

# MariaDB 10.4 Install
# Git History: First added 2019-03-23 (a1708338), Last modified 2025-05-18 (395dea9c)
#MARIADB_INSTALLTENFOUR='n'  

# MariaDB 10.5 Install
# Git History: First added 2022-05-28 (e42d2f41), Last modified 2025-05-18 (395dea9c)
#MARIADB_INSTALLTENFIVE='n'  

# MariaDB 10.6 LTS Install
# Default for EL7/8/9
# Git History: First added 2022-05-28 (e42d2f41), Last modified 2025-05-18 (395dea9c)
#MARIADB_INSTALLTENSIX='y'  

# MariaDB 10.11 LTS Install
# Default for EL10
# Git History: First added 2024-07-03 (cefa63bf), Last modified 2025-05-18 (395dea9c)
#MARIADB_INSTALLTENELEVEN='n'

# MariaDB 11.4 LTS Install
# Git History: First added 2024-07-03 (cefa63bf), Last modified 2025-05-18 (395dea9c)
#MARIADB_INSTALLELEVENFOUR='n'

# MariaDB 11.8 Install
# Git History: Added 2025-10-04 (9e5e58ab)
#MARIADB_INSTALLELEVENEIGHT='n'

# MariaDB RPM Version (Legacy)
# Used only if MDB_INSTALL='y' (not recommended)
# Git History: Added 2014-04-24 (c4b6d98a)
#MDB_VERONLY='5.2.14'
# Git History: Added 2014-04-24 (c4b6d98a)
#MDB_BUILD='122'
# Git History: Added 2014-04-24 (c4b6d98a)
#MDB_VERSION="${MDB_VERONLY}-${MDB_BUILD}"

# MariaDB Previous RPM Version (Legacy)
# For proper upgrades when using RPM method
# Git History: Added 2014-04-24 (c4b6d98a)
#MDB_PREVERONLY='5.2.12'
# Git History: Added 2014-04-24 (c4b6d98a)
#MDB_PREBUILD='115'
# Git History: Added 2014-04-24 (c4b6d98a)
#MDB_PREVERSION="${MDB_PREVERONLY}-${MDB_PREBUILD}"

# MySQL Install
# Install official Oracle MySQL Server (MariaDB alternative)
# Set MDB_INSTALL=y and MYSQL_INSTALL='n' for MariaDB
# Set MYSQL_INSTALL='y' and MDB_INSTALL='n' for MySQL
# Git History: Added 2014-04-24 (c4b6d98a)
#MYSQL_INSTALL='n'

################################################################################
# 31. POSTGRESQL SETTINGS
################################################################################

# PostgreSQL Install
# Set to 'y' to install PostgreSQL server, devel packages, and pdo-pgsql extension
# Git History: First added 2015-09-19 (fac86b32), Last modified 2016-07-01 (90a78784)
#POSTGRESQL='n'

# PostgreSQL Branch Version
# PostgreSQL major version to install
# Reference: https://www.postgresql.org/
# Git History: Added 2019-02-07 (57108912)
#POSTGRESQL_BRANCHVER='17'

################################################################################
# 32. REDIS SERVER SETTINGS
################################################################################

# Redis Server Install
# Install Redis server by default on initial install
# Git History: Added 2021-12-25 (e18faec9)
#REDIS_SERVER_INSTALL='y'  

# Redis Service Disabled
# When 'y', Redis disabled by default via chkconfig off
# Git History: Added 2021-12-25 (e18faec9)
#REDIS_DISABLED='n'

################################################################################
# 33. MEMCACHED SERVER SETTINGS
################################################################################

# Memcached Server Install
# Install Memcached server
# Git History: First added 2014-04-24 (c4b6d98a), Last modified 2016-02-15 (7240145c)
#MEMCACHED_INSTALL='y'  

# Memcached Service Disabled
# When 'y', Memcached disabled by default via chkconfig off
# Git History: First added 2014-04-24 (d962e8b9), Last modified 2016-02-15 (7240145c)
#MEMCACHED_DISABLED='n'  

# libevent Version
# libevent library version for Memcached
# Git History: First added 2014-04-24 (c4b6d98a), Last modified 2016-02-15 (7240145c)
#LIBEVENT_VERSION='2.1.12'  

# Memcached Server Version
# Git History: First added 2014-04-24 (c4b6d98a), Last modified 2016-02-15 (7240145c)
#MEMCACHED_VERSION='1.6.36'  

# Memcached TLS Support
# Enable TLS encryption for Memcached connections
# Reference: https://github.com/memcached/memcached/wiki/ReleaseNotes1513
# Git History: Added 2019-04-17 (84204e1d)
#MEMCACHED_TLS='n'  

# libmemcached YUM Install
# Install libmemcached via YUM instead of source compilation
# Git History: First added 2014-04-24 (d962e8b9), Last modified 2016-02-15 (7240145c)
#LIBMEMCACHED_YUM='y'  

# libmemcached Version
# Version for source compilation (if not using YUM)
# Git History: First added 2014-04-24 (c4b6d98a), Last modified 2016-02-15 (7240145c)
#LIBMEMCACHED_VER='1.0.18'  

# mcperf Version
# Memcached performance testing tool
# Git History: First added 2014-04-24 (c4b6d98a), Last modified 2016-02-15 (7240145c)
#TWEMPERF_VER='0.1.1'

################################################################################
# 34. IMAGEMAGICK SETTINGS
################################################################################

# ImageMagick HEIF Support
# Experimental HEIF image format support
# Git History: Added 2020-04-09 (d65a3756)
#IMAGEMAGICK_HEIF='n'  

# ImageMagick 7
# Install ImageMagick 7.x instead of 6.x (for EL8/EL9)
# Auto-enabled on EL10
# Git History: First added 2024-10-20 (fa2e61a9), Last modified 2025-05-21 (cff78de6)
#IMAGEMAGICK_SEVEN='n'  

# ImageMagick Source Install
# Compile ImageMagick 7 from source with HEIC support
# Git History: Added 2022-12-05 (a82ad587)
#IMAGEMAGICK_SOURCE_INSTALL='n'  

# libde265 Version
# H.265 decoder library for HEIF support
#LIBDE265_VER='1.0.15'

# libheif Version
# HEIF/HEIC image format library
# Git History: Added 2022-12-05 (a82ad587)
#LIBHEIF_VER='1.18.2'

################################################################################
# 35. SERVICE DISABLE FLAGS
################################################################################
#
# When set to 'y', these services are installed but disabled by default
# Can be re-enabled with: service servicename start; chkconfig servicename on
################################################################################

# NSD DNS Server Disabled
# When 'y', NSD disabled by default with chkconfig off
# Git History: First added 2014-04-24 (d962e8b9), Last modified 2016-02-15 (7240145c)
#NSD_DISABLED='n'  

# PHP-FPM Disabled
# When 'y', PHP-FPM disabled by default with chkconfig off
# Git History: First added 2014-04-24 (d962e8b9), Last modified 2016-02-15 (7240145c)
#PHP_DISABLED='n'  

# MySQL Service Disabled
# When 'y', MariaDB/MySQL disabled by default with chkconfig off
# Git History: First added 2014-04-24 (d962e8b9), Last modified 2016-02-15 (7240145c)
#MYSQLSERVICE_DISABLED='n'  

# Pure-FTPd Disabled
# When 'y', Pure-ftpd disabled by default with chkconfig off
# Git History: First added 2015-01-18 (d0f5c8e2), Last modified 2016-02-15 (7240145c)
#PUREFTPD_DISABLED='n'  

################################################################################
# 36. REPOSITORY SETTINGS
################################################################################

# CentALT Repository Disable
# Git History: Added 2014-05-30 (46e642ca)
#CENTALTREPO_DISABLE='y'

# RPMForge Repository Disable
# Git History: Added 2015-06-10 (b54dd034)
#RPMFORGEREPO_DISABLE='n'

# Axivo Repository Disable
# Git History: Added 2014-08-24 (36bbaffe)
#AXIVOREPO_DISABLE='y'

# REMI Repository Disable
# Git History: Added 2014-09-22 (742b887a)
#REMIREPO_DISABLE='n'

# ATrpms Repository Disable
# Git History: Added 2015-07-16 (f999740d)
#ATRPMSREPO_DISABLE='y'

# Varnish Repository Disable
# Git History: Added 2018-04-13 (582946ed)
#VARNISHREPO_DISABLE='y'

# DNF Package Manager Enable
# Enable DNF on EL7 (EL8/9/10 use DNF by default)
# Git History: Added 2017-03-28 (7163c12a)
#DNF_ENABLE='n'

# DNF Copr Repository
# Add Copr repository for DNF on EL7
# Git History: Added 2017-03-28 (04e2f781)
#DNF_COPR='y'

################################################################################
# 37. NSD DNS SERVER SETTINGS
################################################################################

# NSD DNS Server Install
# Install NSD (Name Server Daemon)
# Git History: First added 2014-04-24 (c4b6d98a), Last modified 2016-02-15 (7240145c)
#NSD_INSTALL='n'  

# NSD Version
# Git History: First added 2014-04-24 (c4b6d98a), Last modified 2016-02-15 (7240145c)
#NSD_VERSION='3.2.18'  

################################################################################
# 38. NTP TIME SETTINGS
################################################################################

# NTP Install
# Install Network Time Protocol daemon for time synchronization
# Git History: First added 2014-04-24 (c4b6d98a), Last modified 2016-02-15 (7240145c)
#NTP_INSTALL='y'  

################################################################################
# 39. MAIL SERVICES
################################################################################

# Sendmail Install
# Install Sendmail and mailx
# Set SENDMAIL_INSTALL='y' and POSTFIX_INSTALL='n' for Sendmail
# Git History: First added 2014-04-24 (c4b6d98a), Last modified 2014-06-14 (11141514)
#SENDMAIL_INSTALL='n'

# Postfix Install
# Install Postfix and mailx
# Set POSTFIX_INSTALL='y' and SENDMAIL_INSTALL='n' for Postfix
# Git History: Added 2014-06-14 (11141514)
#POSTFIX_INSTALL=y

################################################################################
# 40. PYTHON SETTINGS
################################################################################

# Python Version
# Python version for CentOS 7 only
# Git History: First added 2014-04-24 (c4b6d98a), Last modified 2016-02-15 (7240145c)
#PYTHON_VERSION='2.7.10'  

# Python Install Alternatives
# For EL8+ to install Python 3.6 and set unversioned binary alternatives
# Git History: Added 2023-04-30 (1dbb7c8a)
#PYTHON_INSTALL_ALTERNATIVES='n'  

# Alternative Python Version Label
# For EL8+ OSes (e.g., python3.6 binary)
# Git History: Added 2023-04-30 (1dbb7c8a)
#ALT_PYTHON_VER_LABEL='3.6'  

# Alternative Python Version
# For EL8+ OSes (e.g., python36 package)
# Git History: Added 2023-04-30 (1dbb7c8a)
#ALT_PYTHON_VER='36'  

################################################################################
# 41. SIEGE BENCHMARK TOOL
################################################################################

# Siege Version
# HTTP load testing and benchmarking tool
# Git History: First added 2014-04-24 (c4b6d98a), Last modified 2016-02-15 (7240145c)
#SIEGE_VERSION='4.1.5'

# Siege Install
# Install Siege benchmarking tool
# Git History: Added 2024-06-26 (7eab8c34)
#SIEGEINSTALL='n'

################################################################################
# 42. DOWNLOAD & MIRROR SETTINGS
################################################################################

# Curl Timeouts
# Timeout values for curl downloads
# Git History: Added 2016-01-28 (cc1e1da2)
#CURL_TIMEOUTS=' --max-time 5 --connect-timeout 5'

# Axel Version
# Axel download accelerator version
# Reference: https://github.com/axel-download-accelerator/axel/releases
# Git History: First added 2016-01-14 (0fff81db), Last modified 2025-05-18 (395dea9c)
#AXEL_VER='2.6'

# Use Axel
# Use Axel download accelerator instead of wget
# Note: Disabled on EL6 due to PHP.net CDN issues
# Git History: First added 2019-02-27 (ea5edb85), Last modified 2019-04-03 (c5697e65)
#USEAXEL='n'

# wget Version
# wget source compile versions by OS
# Git History: First added 2016-07-08 (1cc12f9d), Last modified 2019-09-25 (356c6b3a)
#WGET_VERSION='1.20.3'
# Git History: Added 2019-04-03 (22e45996)
#WGET_VERSION_SEVEN='1.20.3'
# Git History: Added 2021-04-03 (d691f9fb)
#WGET_VERSION_EIGHT='1.21.4'
# Git History: Added 2022-05-28 (2f7b7081)
#WGET_VERSION_NINE='1.21.4'
# Git History: Added 2025-05-18 (a0dfe2af)
#WGET_VERSION_TEN='1.25.0'

# Custom curl/libcurl RPM
# Enable custom curl 7.44+ RPM installation
# Use at own risk - can break system
# Info: http://mirror.city-fan.org/ftp/contrib/sysutils/Mirroring/
# Git History: First added 2015-06-29 (817ca2dc), Last modified 2016-02-15 (7240145c)
#CUSTOM_CURLRPM='n'

# Custom curl RPM Version
# Git History: First added 2015-06-29 (817ca2dc), Last modified 2016-02-15 (7240145c)
#CUSTOM_CURLRPMVER='7.47.1-2.0'  

# Custom libssh2 Version
# Git History: First added 2015-06-29 (817ca2dc), Last modified 2016-02-15 (7240145c)
#CUSTOM_CURLLIBSSHVER='1.6.0-4.0'  

# Custom c-ares Version
# Git History: First added 2015-06-29 (817ca2dc), Last modified 2016-02-15 (7240145c)
#CUSTOM_CURLRPMCARESVER='1.10.0-6.0'  

# Custom curl RPM System URL
# Git History: First added 2015-06-29 (817ca2dc), Last modified 2016-02-15 (7240145c)
#CUSTOM_CURLRPMSYSURL='http://mirror.city-fan.org/ftp/contrib/sysutils/Mirroring'

# Custom curl RPM Library URL
# Git History: First added 2015-06-29 (817ca2dc), Last modified 2016-02-15 (7240145c)
#CUSTOM_CURLRPMLIBURL='http://mirror.city-fan.org/ftp/contrib/libraries'

# Ensure System PHP curl Used
# Prevent custom curl builds from affecting PHP-FPM
# Git History: Added 2024-10-16 (c90b685b)
#ENSURE_SYSTEM_PHP_CURL_USED='n'

# Custom RPM - libevent
# Use custom RPM for libevent (64-bit only, experimental)
# Git History: Added 2014-04-24 (d962e8b9)
#FPMRPM_LIBEVENT='n'

# Custom RPM - Memcached
# Use custom RPM for Memcached (64-bit only, experimental)
# Git History: Added 2014-04-24 (d962e8b9)
#FPMRPM_MEMCACHED='n'

################################################################################
# 43. GEOIP & MAXMIND SETTINGS
################################################################################

# Maxmind GeoLite2 API License Key
# Your Maxmind account API key
# Reference: https://community.centminmod.com/posts/80656/
# Git History: First added 2020-01-02 (2f339d20), Last modified 2023-10-28 (28b8c2de)
#MM_LICENSE_KEY="$GET_CMM_MM_LICENSE_KEY"

# Maxmind CSF Source
# Use Maxmind GeoIP for CSF firewall
# Git History: Added 2020-01-20 (50774ee9)
#MM_CSF_SRC='n'

################################################################################
# 44. TCP SETTINGS
################################################################################

# TCP BBR Congestion Control
# Enable TCP BBR congestion control algorithm
# Git History: Added 2022-09-24 (67b838f9)
#TCP_BBR_ENABLE='n'

# TCP Fast Open
# Enable TCP Fast Open
# Git History: Added 2022-09-25 (2a0e8ae3)
#TCP_FASTOPEN_ENABLE='n'

################################################################################
# 45. WORDPRESS SETTINGS
################################################################################

# WordPress All Plugins
# Install additional WordPress plugins beyond defaults
# Git History: First added 2015-12-02 (a5fdbce7), Last modified 2016-02-15 (7240145c)
#WPPLUGINS_ALL='n'  

# WordPress Cache Enabler Query String
# Cache Enabler plugin cache query string inclusion/exclusion
# Reference: https://community.centminmod.com/posts/85893/
# Git History: Added 2020-09-01 (428b3b72)
#WPCLI_CE_QUERYSTRING_INCLUDED='n'  

# WordPress Super Cache Plugin
# Install WordPress Super Cache plugin
# Reference: https://community.centminmod.com/threads/5102/
# Git History: First added 2015-12-02 (a5fdbce7), Last modified 2016-02-15 (7240145c)
#WPCLI_SUPERCACHEPLUGIN='n'  

# Cache Enabler Legacy Cache
# Cache Enabler 1.4.9 includes query strings, 1.5.1 excludes them
# 'y' = version 1.4.9 (includes query strings)
# 'n' = version 1.5.1 (excludes query strings)
# Git History: Added 2020-10-14 (9f7c82a4)
#CACHE_ENABLER_LEGACY_CACHE='y'

################################################################################
# 46. LET'S ENCRYPT SETTINGS
################################################################################

# Let's Encrypt Auto-Detection
# Auto-detect Let's Encrypt in centmin.sh menu options 2, 22, and /usr/bin/nv
# Requires addons/acmetool.sh to be run first
# Git History: Added 2016-08-30 (556228f5)
#LETSENCRYPT_DETECT='n'

################################################################################
# 47. MENU TRIGGER SETTINGS
################################################################################
#
# Skip certain operations during centmin.sh runs
################################################################################

# Skip Initial PIP Updates
# Git History: Added 2020-06-20 (3aa5ff1a)
#SKIP_INITIAL_PIP_UPDATES='y'

# Skip PIP Updates
# Git History: Added 2020-06-20 (3aa5ff1a)
#SKIP_PIP_UPDATES='y'

# Skip Fix PHP-FPM HTTP Proxy
# Git History: Added 2020-06-20 (3aa5ff1a)
#SKIP_FIXPHPFPM_HTTPPROXY='n'

# Skip Fix WordPress Updater
# Git History: Added 2020-06-20 (3aa5ff1a)
#SKIP_FIXWP_UPDATER='n'

# Skip CSF Maxmind Registration
# Git History: Added 2020-06-20 (3aa5ff1a)
#SKIP_CSF_MAXMIND_REGO='n'

# Skip Check IPv6
# Git History: Added 2020-06-20 (62f1ddf4)
#SKIP_CHECKIPVSIX='n'

# Skip LibC Fix
# Git History: Added 2020-06-20 (62f1ddf4)
#SKIP_LIBC_FIX='n'

################################################################################
# 48. COMPRESSION TOOLS
################################################################################

# Cloudflare zlib Version
# Optimized zlib fork version
# Git History: Added 2017-12-14 (577f71d8)
#CLOUDFLARE_ZLIBVER='1.3.3'

# Nginx zlib Version
# Standard zlib version for Nginx
# Git History: Added 2017-02-05 (171071d2)
#NGINX_ZLIBVER='1.3.1'  

################################################################################
# 49. PERFORMANCE TOOLS
################################################################################

# Google Perftools Source Install
# Install Google Perftools from source
# No longer used in Centmin Mod v1.2.3+ and higher
# Git History: First added 2014-04-24 (c4b6d98a), Last modified 2016-07-09 (37d259b2)
#GPERFTOOLS_SOURCEINSTALL='n'

# Google Perftools tcmalloc Large Pages
# Set larger page size for tcmalloc --with-tcmalloc-pagesize=32
# Git History: Added 2018-02-04 (9fb8e5c8)
#GPERFTOOLS_TMALLOCLARGEPAGES='y'  

# libunwind Version
# Google Perftools specifically requires v0.99
# Git History: First added 2014-04-24 (c4b6d98a), Last modified 2016-07-09 (37d259b2)
#LIBUNWIND_VERSION='1.2.1'  

# Google Perftools Version
# Git History: First added 2014-04-24 (c4b6d98a), Last modified 2016-07-09 (37d259b2)
#GPERFTOOLS_VERSION='2.6.3'  

# Nginx Mimalloc Version
# Alternative memory allocator for Nginx (EL7)
#NGINX_MIMALLOC_VERSION_EL7='2.0.6'

# Nginx Mimalloc Version (EL8+)
# Alternative memory allocator for Nginx (EL8/9/10)
# Git History: Added 2025-10-17 (92df7d77)
#NGINX_MIMALLOC_VERSION='2.2.4'

################################################################################
# 50. SECURITY TOOLS
################################################################################

# Checksec Version
# Security property checker for executables
# Git History: Added 2023-12-01 (533d95fd)
#CHECKSEC_VERSION='3.0.2'

################################################################################
# 51. MOTD & ALERT SETTINGS
################################################################################

# Enable MOTD CSF Message
# Display CSF firewall status in MOTD banner
# Git History: Added 2016-09-16 (bf8792c3)
#ENABLEMOTD_CSFMSG='y'  

# Enable MOTD Links Message
# Display Centmin Mod bookmarks in MOTD
# Git History: Added 2016-09-16 (bf8792c3)
#ENABLEMOTD_LINKSMSG='y'  

# Need Restart Check
# Alert if server reboot required after YUM updates (Fri-Sun only)
# Git History: Added 2023-09-21 (e25e9d28)
#NEEDRESTART_CHECK='n'

################################################################################
# 52. OVERRIDE FLAGS
################################################################################

# Minimum Memory Override
# Override EL8/EL9 minimum memory requirement
# From 1.649GB to 1.43GB
# Reference: https://community.centminmod.com/posts/100321/
# Git History: Added 2024-07-04 (93773df4)
#ISMINMEM_OVERRIDE='n'  

# Minimum Swap Override
# Override EL8/EL9 minimum disk swap requirement
# From 4GB to 2GB
# Reference: https://community.centminmod.com/posts/100321/
# Git History: Added 2024-07-04 (93773df4)
#ISMINSWAP_OVERRIDE='n'  

################################################################################
# 53. MISCELLANEOUS SETTINGS
################################################################################

# Shortcuts Install
# Install command shortcuts for easier management
# Git History: First added 2014-04-24 (c4b6d98a), Last modified 2016-02-15 (7240145c)
#SHORTCUTS='y'  

# PCRE Source Install
# Install PCRE from source (PHP 5.3.8 already includes PCRE)
# Git History: First added 2014-04-24 (c4b6d98a), Last modified 2016-07-09 (37d259b2)
#PCRE_SOURCEINSTALL='n'

# PCRE Version
# Git History: First added 2014-04-24 (c4b6d98a), Last modified 2016-07-09 (37d259b2)
#PCRE_VERSION='8.45'  

# ccache CPP2 Disable
#CCACHE_CPP2_DISABLE='n'

# Parallel Download Mode
# Download tarballs in parallel using subshells
# Git History: Added 2015-12-22 (0c6a7114)
#PARALLEL_MODE=y

################################################################################
# END OF CONFIGURATION FILE
################################################################################
#
# REMINDER: Only uncomment and modify variables you need to override!
#
# For support and documentation:
#   - Official site: https://centminmod.com
#   - Community forum: https://community.centminmod.com
#   - Documentation: https://centminmod.com/upgrade.html#persistent
#
################################################################################
