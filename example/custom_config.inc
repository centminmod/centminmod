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
# 2. Select which variables you want to enable and place in
     /etc/centminmod/custom_config.inc
#
# 3. Edit and uncomment (remove hash # in front of variable)
     ONLY the variables you want to override:
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
# TOTAL VARIABLES DOCUMENTED: 575
# CATEGORIES: 53
# BRANCH: 141.00beta01
# LAST UPDATED: 2025-10-25 (part 5 - fixed typo, added missing variables)
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
# 22. PHP Compilation Settings (24 variables)
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
# 48. Compression Tools (9 variables)
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

# Clang Compiler for Nginx Compilation
# Build Nginx using Clang/LLVM compiler instead of GCC for faster compilation and alternative optimizations.
# AUTOMATIC BEHAVIOR:
#   - EL8/9/10: Auto-installs clang from repos if missing (line 2789-2853)
#   - EL7: Complex version selection (CLANG_FOUR through CLANG_MASTER) (line 2856-2937)
#   - EL6: Legacy Clang 3.4 support (line 3013-3031)
#   - Auto-disables devtoolset when CLANG='y' (line 3060-3091)
#   - Sets compiler paths via CC/CXX environment variables
# CLANG VERSION SELECTION (EL7):
#   - CLANG_MASTER: Latest Clang from source (line 3215)
#   - CLANG_EIGHT: Clang 8.0 (line 3221)
#   - CLANG_SEVEN: Clang 7.0 from llvm-toolset-7.0 (line 3227-3229)
#   - CLANG_SIX: Clang 6.0 (line 3235)
#   - CLANG_FIVE: Clang 5.0 (line 3241)
#   - CLANG_FOUR: Clang 4.0 (line 3247)
# NGINX VERSION COMPATIBILITY:
#   - All Nginx versions compatible
#   - Clang 3.4+ minimum for modern Nginx
# OS COMPATIBILITY:
#   - EL6: Clang 3.4 (legacy, not recommended)
#   - EL7: Clang 4.0 - 8.0 + master (source compile)
#   - EL8: Clang 17.x from AppStream (recommended)
#   - EL9: Clang 17.x+ from AppStream (recommended)
#   - EL10: Clang 18.x+ from AppStream (recommended)
# CONFIGURE FLAGS:
#   - CC=clang CXX=clang++ ./configure ...
#   - Uses CLANG_CCOPT variable for Clang-specific flags
#   - Path set via CC/CXX environment: export CC=/path/to/clang
# CLANG LTO SUPPORT:
#   - CLANG_LTO_ENABLE='y': Enable Link-Time Optimization (line 3121-3130)
#   - CLANG_LTO_FULL='y': Use full LTO instead of ThinLTO (line 3122-3126)
#   - Requires FLTO_COMP='y' on EL8/9/10
# CONFLICTS:
#   - Incompatible with DEVTOOLSET* variables (Clang replaces GCC)
#   - Incompatible with NGINX_DEVTOOLSETGCC='y'
# PERFORMANCE IMPACT:
#   - Compilation speed: 10-30% faster than GCC
#   - Binary size: Similar to GCC
#   - Runtime performance: 0-5% slower than optimized GCC builds
#   - LTO compilation: Significantly slower (2-4x) but better runtime
# COMPILATION FEATURES:
#   - Better error messages than GCC
#   - Faster builds (parallel template instantiation)
#   - Advanced static analysis capabilities
#   - Modern C++ support
# USE CASES:
#   - Development/testing (faster rebuild cycles)
#   - CI/CD pipelines (faster builds)
#   - Alternative to GCC for troubleshooting compilation issues
#   - Clang-specific optimizations research
# RECOMMENDATION: Keep disabled ('n') for production Nginx builds. GCC provides better
#   runtime performance with march=native and devtoolset optimizations. Enable Clang
#   for development/testing where fast compilation is prioritized over runtime performance.
# Related: CLANG_LTO_ENABLE, CLANG_LTO_FULL, DEVTOOLSET*, NGINX_DEVTOOLSETGCC
# Reference: https://clang.llvm.org/
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

# Clang Compiler for PHP-FPM
# Build PHP-FPM using Clang compiler instead of GCC for faster compilation times
# AUTOMATIC BEHAVIOR:
#   - Auto-installs Clang if missing (EL6: /usr/bin/clang, EL7+: /bin/clang)
#   - Reverts to GCC after PHP compilation completes
#   - Bypasses all CPU-specific GCC optimizations (-march=native, AMD znver)
# CONFLICTS:
#   - Mutually exclusive with GCCINTEL_PHP='y' (cannot use both simultaneously)
#   - Disables devtoolset/gcc-toolset when enabled
# PHP VERSION COMPATIBILITY:
#   - PHP 5.x, 7.x, 8.x: Fully compatible
# PERFORMANCE IMPACT:
#   - Compilation: 10-20% faster build times vs GCC
#   - Runtime: 5-10% slower execution vs optimized GCC builds
#   - Binary size: Similar to GCC builds
# USE CASES:
#   - Development environments requiring fast rebuilds
#   - CI/CD pipelines prioritizing build speed
#   - Testing PHP releases without production optimization
# RECOMMENDATION: Use GCC (GCCINTEL_PHP='y') for production, Clang for development
# Related: GCCINTEL_PHP, DEVTOOLSET_PHP, CLANG_MEMCACHED, CLANG_APC
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

# GCC Intel/AMD CPU Optimizations for PHP-FPM
# Enable GCC compiler with CPU-specific optimizations for maximum runtime performance
# AUTOMATIC BEHAVIOR:
#   - Auto-disabled for PHP 5.3 (incompatibility)
#   - Auto-enabled for CentOS 6 + PHP 7.4/8.x (required for compilation)
#   - Auto-disabled for systems with <2 CPU cores (insufficient resources)
#   - Auto-installs devtoolset/gcc-toolset when enabled
# CPU-SPECIFIC OPTIMIZATIONS:
#   - Intel CPUs: -march=native tuning with Intel Core optimizations
#   - AMD CPUs: -march=znver1/znver2/znver3/znver4 (EPYC, Ryzen)
#   - Generic: -march=native for auto-detected CPU features
# COMPILER TOOLSETS BY OS:
#   - EL6: devtoolset-9 (GCC 9.3.1)
#   - EL7: devtoolset-11 (GCC 11.x)
#   - EL8/9: gcc-toolset-12/13/14
#   - EL8.9+/EL9/EL10: gcc-custom-15 (GCC 15.x)
# PHP VERSION COMPATIBILITY:
#   - PHP 5.3: Not supported (auto-disabled)
#   - PHP 5.4-8.4: Fully compatible
# CONFLICTS:
#   - Mutually exclusive with CLANG_PHP='y' (cannot use both)
# PERFORMANCE IMPACT:
#   - Compilation: 2-3x longer build times vs standard GCC
#   - Runtime: 5-15% faster execution vs non-optimized builds
#   - Benchmarks: 10-20% improvement on CPU-intensive workloads
# DEPENDENCIES:
#   - Triggers DEVTOOLSET_PHP='y' automatically
#   - Requires 2+ CPU cores (auto-disabled for single-core systems)
# USE CASES:
#   - Production servers requiring maximum PHP performance
#   - High-traffic sites with CPU-bound PHP workloads
#   - Modern CPUs with advanced instruction sets (AVX2, AVX512)
# RECOMMENDATION: Enable for production (default 'y'), disable for development speed
# Related: DEVTOOLSET_PHP, CLANG_PHP, GCC_OPTLEVEL, NGINX_DEVTOOLSETGCC
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

# Parallel Compilation Mode for Faster Nginx Builds
# Enables parallel make compilation using multiple CPU cores to reduce nginx build time.
# AUTOMATIC BEHAVIOR:
#   - When PARALLEL_MODE='y' and CPU cores > 1: Uses make -j${MAKETHREADS} for parallel compilation (nginx_upgrade.inc:435)
#   - Automatically detects CPU core count via /proc/cpuinfo
#   - MAKETHREADS calculated based on available cores (typically nproc or nproc-1)
#   - Speeds up compilation by 2-8x depending on core count
# NGINX VERSION REQUIREMENTS:
#   - Works with all nginx versions (make parallel compilation standard)
#   - No version-specific requirements
# UPGRADE BEHAVIORS: (from nginx_upgrade.inc)
#   - Line 435: if PARALLEL_MODE='y' and cores > 1 uses make -j${MAKETHREADS}
#   - Single-core systems: Automatically uses single-threaded make
#   - Multi-core systems: Parallel compilation for nginx and dependencies (OpenSSL, PCRE, etc.)
# PATCH BEHAVIORS:
#   - No patches affected (compilation optimization only)
# CONFIGURE FLAGS:
#   - No ./configure flags affected (make-level optimization)
#   - Affects: make, make install stages of compilation
# OS COMPATIBILITY:
#   - EL6/7/8/9/10: Full compatibility across all Enterprise Linux versions
#   - Requires: GNU make 3.81+ (standard on all systems)
#   - CPU detection via /proc/cpuinfo (Linux standard)
# PERFORMANCE IMPACT:
#   - Build time: -50-80% compilation time on multi-core systems
#   - Example: 4-core system builds nginx in ~2 minutes vs 8 minutes (single-threaded)
#   - 16+ core systems: ~1 minute nginx build time
#   - No runtime performance impact (compilation optimization only)
# SECURITY IMPLICATIONS:
#   - No security implications (build-time optimization only)
# CONFLICTS:
#   - No conflicts with other nginx modules
#   - May overwhelm systems with <2GB RAM (high memory usage during parallel build)
# RECOMMENDATION:
#   - Enable for all systems with 2+ CPU cores (99% of modern systems)
#   - Disable if: Single-core system, <2GB RAM, debugging compilation errors
#   - Default MAKETHREADS: Use $(nproc) or $(nproc)-1 for optimal performance
# Related: MAKETHREADS variable, CPU optimization variables
# Reference: GNU make parallel compilation documentation
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

# Devtoolset/GCC-Toolset for PHP Compilation
# Forces use of modern GCC toolsets regardless of OS default compiler version
# AUTOMATIC BEHAVIOR:
#   - Auto-enabled for CentOS 6 + PHP 7.4/8.x (required, system GCC too old)
#   - Auto-enabled when GCCINTEL_PHP='y' (CPU optimizations require modern GCC)
#   - Auto-enabled for PHP 7.3+ compilations (improved C11/C++11 support)
#   - Auto-enabled for PHP 8.x compilations (C11 features required)
#   - Auto-enabled for PHP_PGO='y' builds (PGO requires GCC 8+)
# TOOLSET VERSIONS BY OS:
#   - EL6: devtoolset-9 (GCC 9.3.1)
#   - EL7: devtoolset-11 (GCC 11.x)
#   - EL8: gcc-toolset-12/13/14 (GCC 12.x/13.x/14.x)
#   - EL9: gcc-toolset-13/14 (GCC 13.x/14.x)
#   - EL8.9+/EL9/EL10: gcc-custom-15 (GCC 15.x) if available
# PHP VERSION COMPATIBILITY:
#   - PHP 5.x: Optional (provides C99/C11 features)
#   - PHP 7.0-7.2: Optional but recommended
#   - PHP 7.3-7.4: Auto-enabled (improved compatibility)
#   - PHP 8.x: Auto-enabled (required for C11 features)
# FEATURES PROVIDED:
#   - Modern C/C++ standards (C11, C++11, C++14, C++17)
#   - Advanced compiler optimizations (-flto, -fprofile-generate)
#   - Better CPU instruction support (AVX2, AVX512, BMI2)
#   - Improved link-time optimization (LTO) support
# PERFORMANCE IMPACT:
#   - Compilation: 5-10% longer due to advanced optimizations
#   - Runtime: 3-8% faster execution vs system GCC
#   - Binary compatibility: Same as system compiler
# USE CASES:
#   - PHP 7.4/8.x on CentOS 6 (mandatory)
#   - Production builds with GCCINTEL_PHP='y'
#   - PGO-optimized PHP builds (PHP_PGO='y')
#   - Modern CPU architectures (AMD EPYC, Intel Xeon)
# RECOMMENDATION: Auto-enabled when needed, manual override rarely required
# Related: GCCINTEL_PHP, PHP_PGO, NGINX_DEVTOOLSETGCC, GENERAL_DEVTOOLSETGCC, CRYPTO_DEVTOOLSETGCC
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
#NGINX_VERSION='1.29.3'  

# FreeNginx Installation Switch
# Switch from nginx.org to freenginx.org fork for Nginx compilation and installation.
# AUTOMATIC BEHAVIOR:
#   - Changes download source from nginx-${VERSION}.tar.gz to freenginx-${VERSION}.tar.gz
#   - Modifies source directory path detection (line 404-417, 727-758)
#   - Version detection reads from freenginx-${VERSION}/src/core/nginx.h instead
# NGINX VERSION COMPATIBILITY:
#   - FreeNginx versions track nginx versions (e.g., FreeNginx 1.27.x)
#   - All nginx version checks (DETECT_NGXVER) remain functional
# SOURCE CHANGES:
#   - Download: https://freenginx.org/download/freenginx-${VERSION}.tar.gz
#   - Directory: /svr-setup/freenginx-${VERSION}/
#   - Binary: /usr/local/sbin/nginx (same as nginx.org builds)
# CONFLICTS:
#   - None - fully compatible with all other nginx compilation options
#   - Works with QUIC, AWS-LC, BoringSSL, all modules
# RATIONALE:
#   - FreeNginx is a community fork created in 2024 by former nginx core developers
#   - Maintains API/ABI compatibility with nginx.org
#   - May receive security updates and features faster than nginx.org
# USE CASES:
#   - Organizations preferring community governance over F5/nginx Inc control
#   - Early access to community-driven features
#   - Nginx Inc licensing policy concerns
# RECOMMENDATION: Keep disabled ('n') unless you specifically need FreeNginx fork.
#   Default nginx.org provides wider ecosystem support and documentation.
# Related: None (independent choice from all other variables)
# Reference: https://freenginx.org
# Git History: Added 2024-02-17 (5d801902)
#FREENGINX_INSTALL='n'

# Freenginx Version
# Freenginx version if FREENGINX_INSTALL='y'
# Git History: Added 2024-02-17 (5d801902)
#FREENGINX_VERSION='1.29.3'

# Freenginx Backport Patches
# FreeNginx Security Patches Backported to Standard Nginx
# Applies security and bug fix patches from FreeNginx project to standard Nginx builds for enhanced security.
# AUTOMATIC BEHAVIOR:
#   - When FREENGINX_BACKPORT_PATCHES='y' and NGINX_INSTALL='y' and FREENGINX_INSTALL!='y': Applies FreeNginx patches to standard Nginx (nginx_patch.inc:1088)
#   - Downloads patches from patches/nginx/http2-shutdown-fix.patch (and others)
#   - Only applies patches if nginx version >= 1.25.0 (nginx_patch.inc:1088: DETECT_NGXVER >= 1025000)
#   - Allows standard Nginx installations to benefit from FreeNginx security improvements
#   - Patches apply only to standard nginx (not nginx-angie or FreeNginx itself)
# NGINX VERSION REQUIREMENTS:
#   - Minimum: Nginx 1.25.0+ (nginx_patch.inc:1088: DETECT_NGXVER >= 1025000)
#   - Patches version-specific, may not apply to older nginx releases
#   - Designed for nginx 1.25.x-1.27.x mainline branch
# UPGRADE BEHAVIORS:
#   - No specific upgrade logic in nginx_upgrade.inc
#   - Patches reapplied during nginx recompilation
# PATCH BEHAVIORS: (from nginx_patch.inc)
#   - Line 1088: if NGINX_INSTALL='y' and FREENGINX_INSTALL!='y' and ngver!='angie' and FREENGINX_BACKPORT_PATCHES='y' and patch exists and nginx >= 1.25.0 applies patches
#   - Example patch: http2-shutdown-fix.patch (fixes HTTP/2 connection shutdown race condition)
#   - FreeNginx patches address security issues not yet fixed in stable Nginx
#   - Patches may include: CVE fixes, memory leak fixes, protocol compliance improvements
# CONFIGURE FLAGS:
#   - No specific ./configure flags (patches modify source before compilation)
# OS COMPATIBILITY:
#   - EL6/7/8/9/10: Full compatibility (source code patches)
#   - No additional dependencies
# PERFORMANCE IMPACT:
#   - Negligible: Patches fix bugs without performance overhead
#   - May slightly improve stability by fixing edge-case crashes
# SECURITY IMPLICATIONS:
#   - Improved security: FreeNginx patches may fix CVEs before official Nginx releases
#   - Use case: Get security fixes faster without waiting for nginx.org releases
#   - Trade-off: Less tested than official Nginx patches (FreeNginx community-maintained)
# CONFLICTS:
#   - Only applies to NGINX_INSTALL='y' (not FREENGINX_INSTALL='y')
#   - Does not apply to nginx-angie builds (ngver!='angie')
#   - Requires patch files exist in patches/nginx/ directory
# RECOMMENDATION:
#   - Enable for security-sensitive production sites using standard Nginx
#   - Recommended: Nginx 1.25.x+ users wanting faster security updates
#   - Disable if: Using FreeNginx directly (FREENGINX_INSTALL='y')
#   - Monitor FreeNginx changelog for new security patches
# Related: NGINX_INSTALL, FREENGINX_INSTALL, NGINXPATCH
# Reference: https://freenginx.org/ (security patch source)
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

# Mold Linker for Nginx Compilation
# Use modern Mold linker instead of GNU ld/gold for dramatically faster Nginx linking.
# AUTOMATIC BEHAVIOR:
#   - Calls mold_check() function to verify Mold installation
#   - Auto-installs Mold if not present (line 586-588)
#   - Requires GENERAL_DEVTOOLSETGCC='y' for modern GCC (line 506-515)
#   - Sets linker via -fuse-ld=mold flag
# NGINX VERSION COMPATIBILITY:
#   - All Nginx versions compatible
#   - Requires Nginx build system support for custom linker
# OS COMPATIBILITY:
#   - EL8: Supported (Mold 1.x+)
#   - EL9: Supported (Mold 1.x+)
#   - EL10: Supported (Mold 2.x+)
#   - EL6/EL7: Not recommended (glibc incompatibilities)
# CONFIGURE FLAGS:
#   - --with-ld-opt: -fuse-ld=mold
#   - Uses NGXEXTRA_LDGOLDCCOPT variable
#   - Replaces GNU ld or gold linker
# COMPILER REQUIREMENTS:
#   - GCC 10+ or Clang 12+ required
#   - Triggers devtoolset installation (line 506-515)
#   - Not compatible with EL7 devtoolset-11 or older
# PERFORMANCE IMPACT:
#   - Linking speed: 5-15x faster than GNU ld
#   - Linking speed: 2-5x faster than gold linker
#   - Parallel linking support (multi-threaded)
#   - Most beneficial for large Nginx builds with many modules
# CONFLICTS:
#   - Mutually exclusive with NGX_LDGOLD='y' (cannot use both)
#   - Requires modern GCC (triggers devtoolset even if NGINX_DEVTOOLSETGCC='n')
# LINK-TIME FEATURES:
#   - Multi-threaded linking (uses all CPU cores)
#   - Lower memory usage than gold
#   - Faster LTO linking when CLANG_LTO_ENABLE='y'
# USE CASES:
#   - Large Nginx builds with 10+ dynamic modules
#   - Development environments (faster rebuild cycles)
#   - CI/CD pipelines (reduce build times)
#   - Nginx builds with LTO enabled
# RECOMMENDATION: Enable on EL8+ for significantly faster linking, especially when:
#   - Compiling Nginx with many third-party modules
#   - Using CLANG_LTO_ENABLE='y' or NGINX_FATLTO_OBJECTS='y'
#   - Frequent Nginx recompilation in development
# Related: NGX_LDGOLD, CLANG_LTO_ENABLE, NGINX_DEVTOOLSETGCC
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

# Nginx Dynamic Module Loading Support
# Enable Nginx dynamic module system for loading .so modules at runtime without recompilation.
# AUTOMATIC BEHAVIOR:
#   - Requires Nginx 1.11.5+ (line 418-422) - checks DETECT_NGXVER >= 1011005
#   - Auto-creates /usr/local/nginx/conf/dynamic-modules.conf include file
#   - Auto-adds --with-compat configure flag when enabled (line 418-422)
#   - Auto-generates load_module directives for each NGXDYNAMIC_* module
# NGINX VERSION REQUIREMENTS:
#   - Nginx 1.11.5+ minimum (line 418 DETECT_NGXVER check)
#   - Nginx 1.9.11+: Partial dynamic support (limited modules)
#   - Nginx 1.11.5+: Full dynamic module ABI compatibility
# CONFIGURE FLAGS:
#   - --with-compat (enables module ABI compatibility)
#   - Uses WITHCOMPAT_OPT variable (line 419)
#   - Changes module flags from --add-module to --add-dynamic-module
# DYNAMIC MODULE DIRECTORY:
#   - Modules location: /usr/local/nginx/modules/*.so
#   - Configuration: /usr/local/nginx/conf/dynamic-modules.conf
#   - Include: load_module "modules/module_name.so";
# SUPPORTED DYNAMIC MODULES:
#   - HTTP modules: brotli, zstd, geoip, headers-more, lua, njs, etc.
#   - Stream modules: stream, stream_geoip, stream_realip
#   - Image/filter modules: image_filter, xslt
#   - Third-party modules: Most compatible if built with --with-compat
# MODULE LOADING ORDER:
#   - Controlled by NGXMODULE_ALTORDER='y' for alternative load order
#   - Critical for module dependencies (e.g., lua requires develkit first)
# PERFORMANCE IMPACT:
#   - Runtime: Negligible overhead (modules loaded at startup)
#   - Compilation: Slightly faster (modules built separately)
#   - Maintenance: Easier module enable/disable without full recompilation
# UPGRADE BENEFITS:
#   - Nginx binary upgrades without rebuilding all modules
#   - Module updates independent of Nginx version
#   - Easy module testing (load_module on/off)
# USE CASES:
#   - Production environments with frequent module changes
#   - Testing new modules without full Nginx rebuild
#   - Nginx version upgrades with module compatibility preservation
# CONFLICTS:
#   - Some modules not compatible with dynamic loading (requires static compilation)
#   - Module ABI compatibility must match Nginx version
# RECOMMENDATION: Enable on production Nginx 1.11.5+ deployments for maximum
#   flexibility. Allows easy module enable/disable without recompilation. Use
#   NGXDYNAMIC_* variables to control which modules compile as dynamic.
# Related: NGXDYNAMIC_* (30+ variables), NGXMODULE_ALTORDER, NGXDYNAMIC_MANUALOVERRIDE
# Reference: https://nginx.org/en/docs/ngx_core_module.html#load_module
# Git History: Analysis added 2025-10-25, original Git history not available
#DYNAMIC_SUPPORT='y'

# Dynamic Module Manual Override Protection
# Prevent automatic removal of dynamic module .so files when switching from dynamic to static compilation.
# AUTOMATIC BEHAVIOR:
#   - When 'n' (default): Auto-removes old .so files when NGXDYNAMIC_MODULE='n' (line 3006-3039 pattern)
#   - When 'y': Preserves .so files even when switching to static compilation
#   - Affects cleanup logic for all NGXDYNAMIC_* variables
# MODULE CLEANUP LOGIC (when NGXDYNAMIC_MANUALOVERRIDE='n'):
#   - If NGXDYNAMIC_MODULE='y' previously created .so file
#   - Then user changes to NGXDYNAMIC_MODULE='n' (static compilation)
#   - Old dynamic .so file automatically deleted (line 3005-3018 examples)
# EXAMPLE CLEANUP (NGXDYNAMIC_BROTLI):
#   if [[ -f /usr/local/nginx/modules/ngx_http_brotli_filter_module.so && "$NGXDYNAMIC_MANUALOVERRIDE" != [yY] ]]; then
#     rm -rf /usr/local/nginx/modules/ngx_http_brotli_filter_module.so
#   fi
# USE CASES FOR NGXDYNAMIC_MANUALOVERRIDE='y':
#   - Manual management of dynamic module files
#   - Custom module testing (keep both static and dynamic versions)
#   - Prevent accidental deletion of custom-compiled modules
#   - Module rollback scenarios (preserve old .so files)
# MODULES AFFECTED:
#   - All NGXDYNAMIC_* variables respect this override
#   - brotli, zstd, lua, njs, geoip, headers-more, stream, etc.
# RECOMMENDATION: Keep disabled ('n') for automatic cleanup behavior. Only enable
#   if you manually manage dynamic module files and want to prevent automatic deletion
#   when switching compilation modes.
# Related: DYNAMIC_SUPPORT, all NGXDYNAMIC_* variables
# Reference: Code pattern at line 3006-3018 (repeated for each dynamic module)
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
# Nginx JavaScript (NJS) Module Integration
# Installs njs (nginx JavaScript) module for embedding JavaScript in nginx configuration using ECMAScript subset.
# AUTOMATIC BEHAVIOR:
#   - When NGINX_NJS='y': Clones njs repository from GitHub and checks out specific tag version (nginx_configure.inc:3413,3448)
#   - Automatically compiles njs binary and libnjs library
#   - Installs njs command-line tool to /usr/local/nginx/sbin/njs (nginx_install.inc:608, nginx_upgrade.inc:1523)
#   - DYNAMIC_SUPPORT='y': Builds njs as loadable module (.so file)
#   - Static build: Compiles njs directly into nginx binary
# NGINX VERSION REQUIREMENTS:
#   - Minimum: Nginx 1.7.4+ (njs module support introduced)
#   - Recommended: Nginx 1.19.0+ for modern njs features
#   - njs version tied to nginx version compatibility (NGINX_NJS_VER variable)
# UPGRADE BEHAVIORS: (from nginx_upgrade.inc)
#   - Line 1523: if NGINX_NJS='y' and njs binary exists, copies njs CLI to /usr/local/nginx/sbin/
#   - Recompiles njs module with new nginx version during upgrades
#   - Preserves existing njs scripts in /usr/local/nginx/conf/njs/
# PATCH BEHAVIORS:
#   - No specific njs patches in nginx_patch.inc (njs is standalone git repository)
#   - njs version updates tracked via NGINX_NJS_VER git tags
# CONFIGURE FLAGS:
#   - DYNAMIC_SUPPORT='y': --add-dynamic-module=../njs/nginx (creates ngx_http_js_module.so)
#   - Static build: --add-module=../njs/nginx (compiled into nginx binary)
#   - Both: Enables js_import, js_set, js_content directives in nginx.conf
# OS COMPATIBILITY:
#   - EL6: Limited support (requires gcc-6+ for C++11)
#   - EL7: Full support with devtoolset-11+
#   - EL8/9/10: Native gcc-11+ support, no devtoolset needed
#   - Requires: pcre-devel or pcre2-devel, zlib-devel
# PERFORMANCE IMPACT:
#   - CPU: +5-15% CPU usage for JavaScript execution (lighter than Lua)
#   - Memory: +10-30MB per worker for njs VM
#   - Latency: +1-5ms per request (JavaScript evaluation overhead)
#   - njs is faster than Lua for simple operations, slower for complex logic
#   - Recommended: Use for lightweight request/response manipulation
# SECURITY IMPLICATIONS:
#   - njs is a restricted JavaScript subset (no filesystem, network access by default)
#   - Safer than lua-nginx-module due to limited syscall access
#   - Cannot execute arbitrary system commands (sandboxed environment)
#   - Suitable for: request routing, header manipulation, authentication logic
# CONFLICTS:
#   - Compatible with all nginx modules (designed for integration)
#   - No conflicts with NGINX_OPENRESTY or ORESTY_LUANGINX (can coexist)
#   - Lighter alternative to Lua for simple scripting tasks
# RECOMMENDATION:
#   - Enable for JavaScript-based request routing and manipulation
#   - Recommended for: JWT validation, API gateway logic, dynamic routing
#   - Disable if: Only serving static content or using Lua modules extensively
#   - Use over Lua if: Team has JavaScript expertise, need sandboxed scripting
#   - Pair with DYNAMIC_SUPPORT='y' for easier module updates without nginx recompilation
# Related: NGINX_NJS_VER, DYNAMIC_SUPPORT, NGINX_OPENRESTY, ORESTY_LUANGINX
# Reference: https://nginx.org/en/docs/njs/, https://github.com/nginx/njs
# Git History: Added with njs module support ~2015-2016
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

# SPDY Protocol Support (Deprecated, HTTP/2 Predecessor)
# Controls legacy SPDY protocol support, automatically disabled when HTTP/2 is enabled.
# AUTOMATIC BEHAVIOR:
#   - When NGINX_HTTP2='y' and nginx >= 1.9.7: Automatically sets NGINX_SPDY='n' (nginx_patch.inc:1166)
#   - NGINX_SPDYPATCHED='y' automatically enables NGINX_SPDY='y' (nginx_configure.inc:921-922)
#   - Disables itself when HTTP/2 is detected as they are mutually exclusive
#   - Nginx 1.9.5-1.9.6 allows both SPDY and HTTP/2 simultaneously (transition period)
# NGINX VERSION REQUIREMENTS:
#   - Supported: Nginx 1.5.10 through 1.9.4 (official SPDY support)
#   - Nginx 1.9.5-1.9.6: Transitional - both SPDY and HTTP/2 supported
#   - Nginx 1.9.7+: SPDY removed, HTTP/2 only
#   - HTTP/2 available: Nginx 1.9.5+ (supersedes SPDY)
# UPGRADE BEHAVIORS: (from nginx_upgrade.inc)
#   - Line 800-801: if NGINX_SPDYPATCHED='n' disables patches by removing ngx_http_spdy_filter from auto/modules
#   - Line 1290: When upgrading to 1.9.7+ with NGINX_SPDYPATCHED='y', automatically uses HTTP/2 instead
#   - Line 1326: elif [[ "$NGINX_HTTP2" = [nN] || "$NGINX_SPDY" = [yY] ]] uses SPDY configure flags
#   - Upgrade from SPDY to HTTP/2 happens automatically at Nginx 1.9.7+ threshold
# PATCH BEHAVIORS: (from nginx_patch.inc)
#   - Line 1166: Forces NGINX_SPDY='n' when nginx >= 1.27.0 (HTTP/2 mandatory)
#   - Line 1182: if NGINX_SPDYPATCHED='y' applies SPDY continuation patches for nginx < 1.9.7
#   - SPDY patch directory: $CUR_DIR/patches/nginx/spdy/ (version-specific patches)
# CONFIGURE FLAGS:
#   - NGINX_SPDY='y': --with-http_spdy_module
#   - NGINX_HTTP2='y' (supersedes): --with-http_v2_module
#   - Both: --with-http_spdy_module --with-http_v2_module (nginx 1.9.5-1.9.6 only)
# OS COMPATIBILITY:
#   - EL6: Nginx 1.5.10-1.9.4 support SPDY (OpenSSL 1.0.1e+ required for NPN)
#   - EL7/8/9/10: SPDY deprecated, HTTP/2 recommended (ALPN support)
#   - All distributions: HTTP/2 preferred over SPDY since 2015
# PERFORMANCE IMPACT:
#   - SPDY: Multiplexing, header compression, server push (Nginx 1.5.10-1.9.4)
#   - HTTP/2: Better performance than SPDY, industry standard since RFC 7540 (2015)
#   - No performance benefit to enabling SPDY on modern systems
# SECURITY IMPLICATIONS:
#   - SPDY protocol has known vulnerabilities (CVE-2014-3566 POODLE affects SPDY/3)
#   - HTTP/2 more secure with mandatory TLS 1.2+ and modern cipher suites
#   - SPDY uses NPN (Next Protocol Negotiation), HTTP/2 uses ALPN (Application-Layer Protocol Negotiation)
# CONFLICTS:
#   - Mutually exclusive with NGINX_HTTP2='y' on nginx 1.9.7+
#   - Requires OpenSSL 1.0.1+ with NPN support
#   - Cannot coexist with HTTP/2 on same port for nginx 1.9.7+
# RECOMMENDATION:
#   - DO NOT ENABLE - SPDY is deprecated and removed from modern Nginx
#   - Use NGINX_HTTP2='y' instead for all modern deployments
#   - Only enable if running legacy nginx < 1.9.7 for old client compatibility
#   - Upgrade to Nginx 1.25.x+ with HTTP/2 for best performance and security
# Related: NGINX_HTTP2, NGINX_SPDYPATCHED, NGINX_HTTP3, NGINX_QUIC_SUPPORT
# Reference: https://www.chromium.org/spdy/ (deprecated), https://http2.github.io/
# Git History: Added ~2013 with Nginx 1.5.10 SPDY support, deprecated since 2015
#NGINX_SPDY='n'  

# SPDY Protocol Patch Application Control
# Controls whether SPDY protocol patches are applied to nginx source code (deprecated feature).
# AUTOMATIC BEHAVIOR:
#   - When NGINX_SPDYPATCHED='y': Automatically sets NGINX_SPDY='y' (nginx_configure.inc:921-922)
#   - Applies SPDY continuation frame patches from $CUR_DIR/patches/nginx/spdy/
#   - Modifies nginx auto/modules file to enable/disable ngx_http_spdy_filter module
#   - When NGINX_HTTP2='y' and nginx >= 1.9.7: Automatically uses HTTP/2 instead (nginx_upgrade.inc:1290)
# NGINX VERSION REQUIREMENTS:
#   - Effective versions: Nginx 1.5.10 through 1.9.6
#   - Nginx 1.9.7+: SPDY removed, this variable has no effect
#   - Patches only applied if ngx_http_spdy_filter detected in auto/modules file
# UPGRADE BEHAVIORS: (from nginx_upgrade.inc)
#   - Line 800-801: if NGINX_SPDYPATCHED='n' removes ngx_http_spdy_filter from auto/modules during upgrade
#   - Line 1290: if NGINX_HTTP2='y' and NGINX_SPDYPATCHED='y' and nginx >= 1.9.7, uses HTTP/2 configure flags
#   - Line 1326: elif NGINX_SPDY='y' uses SPDY-specific configure options
#   - Automatically transitions from SPDY to HTTP/2 during version upgrades
# PATCH BEHAVIORS: (from nginx_patch.inc)
#   - Line 1182: if NGINX_SPDYPATCHED='y' applies SPDY patches (nginx < 1.9.7 only)
#   - Applies continuation frame handling patches for SPDY/3.1 protocol
#   - Patches located in: patches/nginx/spdy/*.patch files
# CONFIGURE FLAGS:
#   - When enabled: --with-http_spdy_module (nginx < 1.9.7)
#   - When nginx >= 1.9.7: Ignored, HTTP/2 flags used instead
#   - SPDYOPT variable set to ' --with-http_spdy_module' or empty
# OS COMPATIBILITY:
#   - EL6: Works with nginx 1.5.10-1.9.6 (requires OpenSSL 1.0.1+)
#   - EL7/8/9/10: Deprecated, no effect on modern nginx builds
#   - All distributions: HTTP/2 supersedes SPDY functionality
# PERFORMANCE IMPACT:
#   - No performance impact on modern nginx (feature removed)
#   - Historical: SPDY provided ~50% faster page loads vs HTTP/1.1 (2013-2015 era)
#   - HTTP/2 provides equivalent or better performance
# SECURITY IMPLICATIONS:
#   - SPDY has known security issues (POODLE, CRIME, BREACH vulnerabilities)
#   - HTTP/2 more secure with TLS 1.2+ requirement and modern cipher suites
#   - Enabling on old nginx versions may expose to deprecated protocol attacks
# CONFLICTS:
#   - Mutually exclusive with NGINX_HTTP2='y' on nginx 1.9.7+
#   - Cannot be used with NGINX_SPDY='n' (SPDYPATCHED requires SPDY='y')
#   - Requires NPN (Next Protocol Negotiation) in OpenSSL
# RECOMMENDATION:
#   - DO NOT ENABLE - SPDY is deprecated and insecure
#   - Use NGINX_HTTP2='y' with nginx 1.25.x+ for modern protocol support
#   - Only relevant for historical nginx 1.5.x-1.9.6 deployments
#   - Disable for all new installations and upgrades
# Related: NGINX_SPDY, NGINX_HTTP2, NGINX_HTTP3, NGINX_QUIC_SUPPORT
# Reference: https://nginx.org/en/CHANGES (nginx 1.9.7 removed SPDY)
# Git History: Added with SPDY support ~2013, deprecated since nginx 1.9.7 (2015)
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
# Nginx Cache Purge Module Integration
# Adds ability to purge cached content via HTTP PURGE/DELETE requests for fastcgi_cache and proxy_cache.
# AUTOMATIC BEHAVIOR:
#   - When NGINX_CACHEPURGE='y': Downloads ngx_cache_purge-${NGINX_CACHEPURGEVER} module (nginx_configure.inc:1220)
#   - Adds cache_purge directives support to nginx configuration
#   - Enables PURGE/DELETE HTTP methods for selective cache invalidation
#   - No automatic vhost configuration, requires manual cache_purge location block
# NGINX VERSION REQUIREMENTS:
#   - Minimum: Nginx 1.0.0+ (basic cache purge support)
#   - Nginx 1.11.6+ with cache purge 2.3: Requires compatibility patches (nginx_patch.inc:738)
#   - Works with all modern nginx versions (1.25.x+)
# UPGRADE BEHAVIORS: (from nginx_upgrade.inc)
#   - No specific upgrade logic in nginx_upgrade.inc
#   - Module recompiled with nginx during upgrades
# PATCH BEHAVIORS: (from nginx_patch.inc)
#   - Line 738: if NGINX_CACHEPURGE='y' and nginx >= 1.11.6 and cache purge 2.3 applies patches
#   - Patches for cache API changes in newer nginx versions
# CONFIGURE FLAGS:
#   - --add-module=../ngx_cache_purge-${NGINX_CACHEPURGEVER}
#   - Compatible with both fastcgi_cache and proxy_cache directives
# OS COMPATIBILITY:
#   - EL6/7/8/9/10: Full compatibility across all Enterprise Linux versions
#   - No special dependencies, works with standard nginx build
# PERFORMANCE IMPACT:
#   - Minimal overhead: Only active when PURGE/DELETE request received
#   - Recommended for: CDN origin servers, API caching, frequent content updates
#   - Allows instant cache invalidation without waiting for cache expiration
# SECURITY IMPLICATIONS:
#   - PURGE requests should be restricted by IP address (allow internal IPs only)
#   - Example nginx config: location ~ /purge(/.*) { allow 127.0.0.1; deny all; proxy_cache_purge; }
#   - Unrestricted PURGE access allows DoS attacks (purge entire cache)
# CONFLICTS:
#   - No conflicts with other nginx modules
#   - Works with fastcgi_cache, proxy_cache, uwsgi_cache
# RECOMMENDATION:
#   - Enable for sites using nginx caching (fastcgi_cache or proxy_cache)
#   - Recommended for: WordPress with nginx fastcgi_cache, API caching
#   - Restrict PURGE access to trusted IPs only
#   - Example usage: curl -X PURGE https://example.com/cached-page
# Related: WP_FASTCGI_CACHE, NGINX_CACHEPURGEVER
# Reference: https://github.com/nginx-modules/ngx_cache_purge
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

# Brotli Compression Module for Nginx
# Add Google Brotli compression support to Nginx for superior compression ratios vs gzip.
# AUTOMATIC BEHAVIOR:
#   - Checks for existing dynamic modules and removes old versions (line 3006-3039)
#   - Creates /usr/local/nginx/conf/brotli_inc.conf configuration include
#   - Auto-enables/disables include directive based on NGINX_LIBBROTLI setting
#   - NGINX_LIBBROTLISTATIC='y' enables static-only module (line 3019-3021)
# NGINX VERSION COMPATIBILITY:
#   - Nginx 1.11.5+ for dynamic module support (with DYNAMIC_SUPPORT)
#   - All Nginx versions for static compilation
# OS COMPATIBILITY:
#   - EL6/7/8/9/10: All supported
#   - Requires libbrotli library compilation
# CONFIGURE FLAGS:
#   - Static: --add-module=../ngx_brotli
#   - Dynamic: --add-dynamic-module=../ngx_brotli (when NGXDYNAMIC_BROTLI='y')
#   - Uses BROTLIOPT variable
# DYNAMIC MODULE FILES:
#   - /usr/local/nginx/modules/ngx_http_brotli_filter_module.so (dynamic compression)
#   - /usr/local/nginx/modules/ngx_http_brotli_static_module.so (pre-compressed files)
#   - Auto-cleanup of .old module versions (line 3012-3017, 3027-3038)
# STATIC-ONLY MODE:
#   - NGINX_LIBBROTLISTATIC='y' sets NGX_BROTLI_STATIC_MODULE_ONLY=1 (line 3020)
#   - Compiles only brotli_static module (serve .br files)
#   - Excludes dynamic brotli compression filter
# PERFORMANCE IMPACT:
#   - Compression ratio: 15-25% better than gzip level 9
#   - Compression speed: Slower than gzip (CPU intensive)
#   - Decompression speed: Similar to gzip
#   - Best for pre-compressed static assets (.br files)
# COMPRESSION LEVELS:
#   - Levels 0-11 (0=fastest, 11=best compression)
#   - Level 4 ≈ gzip 9 compression ratio but slower
#   - Level 11: 20-30% better than gzip 9 but 10-20x slower
# HTTP FEATURES:
#   - brotli_static on: Serve pre-compressed .br files
#   - brotli on: Dynamic brotli compression
#   - brotli_types: MIME types to compress
#   - brotli_comp_level: Compression level 0-11
# BROWSER SUPPORT:
#   - Chrome 50+ (2016): Native brotli support
#   - Firefox 44+ (2016): Native brotli support
#   - Safari 11+ (2017): Native brotli support
#   - Edge 15+ (2017): Native brotli support
# USE CASES:
#   - Static asset compression (CSS, JS, HTML) with .br pre-compression
#   - Modern browsers (universal support since 2017)
#   - Bandwidth-constrained deployments
#   - CDN origin compression
# RECOMMENDATION: Enable for production deployments. Use NGINX_LIBBROTLISTATIC='y'
#   with pre-compressed .br files for best performance (avoid dynamic compression CPU cost).
#   Use dynamic module (NGXDYNAMIC_BROTLI='y') for easier enable/disable.
# Related: NGXDYNAMIC_BROTLI, NGINX_LIBBROTLISTATIC, NGINX_ZSTD, CLOUDFLARE_ZLIB
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

# Zstandard (zstd) Compression Module for Nginx
# Add Zstandard compression support to Nginx for superior compression ratios and performance.
# AUTOMATIC BEHAVIOR:
#   - Calls nginx_zstd_setup() function (line 3043, 3086)
#   - Checks DYNAMIC_SUPPORT and NGXDYNAMIC_ZSTD for dynamic vs static module (line 3046-3098)
#   - Auto-creates /usr/local/nginx/conf/zstd_inc.conf include file
#   - Dynamic module: Auto-adds load_module to dynamic-modules.conf (line 3058-3060)
# NGINX VERSION COMPATIBILITY:
#   - Nginx 1.11.5+ for dynamic module support
#   - All modern Nginx versions compatible for static module
# OS COMPATIBILITY:
#   - EL6/7/8/9/10: All supported
#   - Requires zstd library 1.3.0+ installed
# CONFIGURE FLAGS:
#   - Dynamic: --add-dynamic-module=../zstd-nginx-module (line 3047, 3091)
#   - Static: --add-module=../zstd-nginx-module (line 3068, 3095)
#   - --with-ld-opt: -L/usr/local/nginx-dep/lib (line 3048)
#   - --with-cc-opt: -I/usr/local/nginx-dep/include (line 3049)
# DYNAMIC MODULE LOADING:
#   - Creates /usr/local/nginx/modules/ngx_http_zstd_filter_module.so
#   - Creates /usr/local/nginx/modules/ngx_http_zstd_static_module.so
#   - Auto-cleanup of .old module files (line 3061-3066)
# PERFORMANCE IMPACT:
#   - Compression speed: 2-5x faster than gzip at similar ratios
#   - Decompression speed: 3-10x faster than gzip
#   - Compression ratio: 10-30% better than gzip level 9
#   - CPU usage: Lower than gzip at equivalent compression levels
# COMPRESSION LEVELS:
#   - Levels 1-19: Real-time compression (1=fastest, 19=best ratio)
#   - Level 3 ≈ gzip 6 compression ratio but 3x faster
#   - Level 9 ≈ gzip 9 compression ratio but 2x faster
# HTTP FEATURES:
#   - zstd_static on: Serve pre-compressed .zst files
#   - zstd on: Dynamic zstd compression (like gzip on)
#   - zstd_types: MIME types to compress
#   - zstd_comp_level: Compression level 1-19
# BROWSER SUPPORT:
#   - Chrome 123+ (2024): Native zstd support
#   - Firefox 126+ (2024): Native zstd support
#   - Safari: Not yet supported (as of 2024)
# USE CASES:
#   - Modern browsers (Chrome/Firefox 2024+)
#   - API responses (JSON, XML compression)
#   - Static asset compression (.zst pre-compression)
#   - Superior compression for large payloads (>10KB)
# RECOMMENDATION: Enable for modern deployments serving Chrome/Firefox users.
#   Provides significant performance improvement over gzip. Use dynamic module
#   (NGXDYNAMIC_ZSTD='y') for easier enable/disable without recompilation.
# Related: NGXDYNAMIC_ZSTD, NGINX_LIBBROTLI, CLOUDFLARE_ZLIB
# Reference: https://github.com/tokers/zstd-nginx-module
# Git History: Added 2024-06-20 (9c1acda3)
#NGINX_ZSTD='n'  

# Nginx PageSpeed Module
# Google PageSpeed Module Integration
# Installs ngx_pagespeed module for automatic web page optimization (JavaScript, CSS, images).
# AUTOMATIC BEHAVIOR:
#   - When NGINX_PAGESPEED='y': Downloads ngx_pagespeed-${NGXPGSPEED_VER} and PSOL binaries (nginx_upgrade.inc:456,463,470,709)
#   - Automatically disabled on ARM/non-x86_64 architectures (nginx_pagespeed.inc:495: uname -m = 'x86_64')
#   - Disabled for FreeNginx builds: NGINX_PAGESPEED='n' (nginx_upgrade.inc:651)
#   - When disabled: Comments out pagespeed directives in all existing vhosts (nginx_pagespeed.inc:463-493)
#   - Creates /usr/local/nginx/conf/pagespeed/ directory for cache and config files
# NGINX VERSION REQUIREMENTS:
#   - Minimum: Nginx 1.10.0+ (ngx_pagespeed v1.13.35.1+)
#   - Recommended: Nginx 1.25.x+ with ngx_pagespeed latest-stable branch
#   - CentOS 6: Requires special F_SETPIPE_SZ ifdef patches (nginx_pagespeed.inc:3-5)
#   - Automatically disabled on nginx-angie and FreeNginx builds
# UPGRADE BEHAVIORS: (from nginx_upgrade.inc)
#   - Line 456,463,470: Downloads ngx_pagespeed source during upgrade if enabled
#   - Line 651: Forces NGINX_PAGESPEED='n' on FreeNginx upgrades (incompatibility)
#   - Line 709: if NGINX_PAGESPEED='y' downloads PSOL (PageSpeed Optimization Libraries) binaries
#   - Preserves PageSpeed cache directory during upgrades
# PATCH BEHAVIORS: (from nginx_patch.inc)
#   - No specific patches in nginx_patch.inc (handled by nginx_pagespeed.inc module)
# CONFIGURE FLAGS:
#   - --add-module=../ngx_pagespeed-${NGXPGSPEED_VER}/
#   - Requires downloading PSOL binaries matching ngx_pagespeed version
#   - CentOS 6: Extra CFLAGS for F_SETPIPE_SZ compatibility
#   - CentOS 7/8/9/10: Standard integration, no special flags
# OS COMPATIBILITY:
#   - EL6: Limited support, requires F_SETPIPE_SZ patches (nginx_pagespeed.inc:3)
#   - EL7/8/9/10: Full support with modern ngx_pagespeed versions
#   - x86_64 only: ARM/ARM64 not supported by PSOL binaries
#   - AlmaLinux/Rocky: Identical to CentOS compatibility
# PERFORMANCE IMPACT:
#   - CPU: +10-30% CPU usage for real-time page optimization
#   - Memory: +50-200MB per worker for image processing and caching
#   - Disk: Requires 100MB-2GB cache directory (/usr/local/nginx/conf/pagespeed/)
#   - Can significantly reduce page load times (30-50%) via minification and optimization
# SECURITY IMPLICATIONS:
#   - PageSpeed fetches and processes external resources (CSS, JS from CDNs)
#   - Can expose backend server IPs if not configured properly
#   - Requires careful configuration to avoid cache poisoning
#   - Google discontinued active development in 2020 (security updates limited)
# CONFLICTS:
#   - Incompatible with FREENGINX_INSTALL='y' (nginx_upgrade.inc:651)
#   - May conflict with NGINX_OPENRESTY='y' in some scenarios (nginx_upgrade.inc:494)
#   - Requires substantial memory, may conflict with low-memory environments
# RECOMMENDATION:
#   - NOT RECOMMENDED for new deployments (Google discontinued in 2020)
#   - Use modern alternatives: Cloudflare, Fastly CDN, or manual optimization
#   - If enabled: Monitor CPU/memory usage, configure aggressive cache expiration
#   - Disable for API servers, WebSocket endpoints, or low-latency requirements
# Related: NGINX_PAGESPEEDGITMASTER, NGXPGSPEED_VER, NGINX_PAGESPEEDPSOL_VER
# Reference: https://www.modpagespeed.com/ (deprecated)
# Git History: First added 2014-04-24 (c4b6d98a), Last modified 2016-02-15 (7240145c)
#NGINX_PAGESPEED='n'  

# Google PageSpeed Git Master Branch Installation
# Installs latest ngx_pagespeed module from GitHub master branch instead of stable release.
# AUTOMATIC BEHAVIOR:
#   - When NGINX_PAGESPEEDGITMASTER='y': Clones ngx_pagespeed from GitHub master branch (nginx_upgrade.inc:525)
#   - Automatically downloads latest PSOL (PageSpeed Optimization Libraries) from master
#   - Overrides NGXPGSPEED_VER variable with git master commit hash
#   - Skips version-specific compatibility checks (uses bleeding-edge code)
# NGINX VERSION REQUIREMENTS:
#   - Minimum: Nginx 1.10.0+ (same as stable PageSpeed)
#   - Master branch may have untested nginx version compatibility
#   - May work with nginx-mainline versions before official PageSpeed support
# UPGRADE BEHAVIORS: (from nginx_upgrade.inc)
#   - Line 525: if NGINX_PAGESPEEDGITMASTER='y' git clones instead of tarball download
#   - Always pulls latest master commit during upgrades (unpredictable versioning)
#   - May break existing installations if master has breaking changes
# PATCH BEHAVIORS:
#   - No specific patches (uses upstream master branch as-is)
#   - May include unreleased patches not in stable releases
# CONFIGURE FLAGS:
#   - Same as NGINX_PAGESPEED='y': --add-module=../ngx_pagespeed-master/
#   - Uses latest PSOL binaries compatible with master branch
# OS COMPATIBILITY:
#   - Same as NGINX_PAGESPEED (EL7/8/9/10, x86_64 only)
#   - May have additional OS-specific bugs not present in stable releases
# PERFORMANCE IMPACT:
#   - Same as NGINX_PAGESPEED (CPU +10-30%, Memory +50-200MB)
#   - May have performance regressions from experimental features
# SECURITY IMPLICATIONS:
#   - Higher risk: Master branch may contain unpatched vulnerabilities
#   - Fewer eyes on code compared to stable releases
#   - Google discontinued active development (master branch stale since 2020)
# CONFLICTS:
#   - Same conflicts as NGINX_PAGESPEED
#   - Additional risk of incompatibility with production nginx versions
# RECOMMENDATION:
#   - DO NOT USE in production environments
#   - Testing/development only for evaluating new PageSpeed features
#   - Use NGINX_PAGESPEED='y' with stable release versions instead
#   - Consider modern alternatives (Cloudflare, CDN optimization)
# Related: NGINX_PAGESPEED, NGXPGSPEED_VER
# Reference: https://github.com/apache/incubator-pagespeed-ngx (archived)
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
# PCRE2 Library Support for Nginx Regex Operations
# Controls whether nginx uses PCRE2 (modern) or PCRE1 (legacy) library for regex pattern matching.
# AUTOMATIC BEHAVIOR:
#   - When NGINX_PCRE_TWO='y' and nginx >= 1.21.5: Downloads and compiles PCRE2 10.x+ (nginx_upgrade.inc:28-30,76,772)
#   - Automatically installs pcre2-devel package and compiles from source (nginx_install.inc:180, nginx_configure.inc:506)
#   - Creates /usr/local/nginx-dep/ directory for PCRE2 installation
#   - Forces PCRE2 usage via --with-pcre= configure flag
#   - When nginx < 1.21.5: Automatically falls back to PCRE1 (nginx_configure.inc:592,597)
# NGINX VERSION REQUIREMENTS:
#   - Minimum: Nginx 1.21.5+ (PCRE2 support introduced)
#   - Nginx < 1.21.5: Automatically uses PCRE1 regardless of this setting
#   - Recommended: Nginx 1.25.x+ for mature PCRE2 integration
#   - Version detection: if DETECT_NGXVER >= 1021005 enables PCRE2 (nginx_upgrade.inc:76,772)
# UPGRADE BEHAVIORS: (from nginx_upgrade.inc)
#   - Line 28-30: checks for NGINX_PCRE_TWO='y' during upgrade preparation
#   - Line 76,153: if nginx >= 1.21.5 and NGINX_PCRE_TWO='y' displays status and config
#   - Line 83,160: Echoes NGINX_PCRE_TWO=$NGINX_PCRE_TWO in debug output
#   - Line 112: elif NGINX_PCRE_TWO != 'y' uses PCRE1 during upgrade
#   - Line 772: if DETECT_NGXVER >= 1021005 and NGINX_PCRE_TWO='y' uses PCRE2 source
#   - Line 991,1120: Echoes NGINX_PCRE_TWO value during configure phase
#   - Line 1013,1143: if PCRE2 install log exists, skips reinstallation during upgrade
# PATCH BEHAVIORS:
#   - No specific PCRE2 patches in nginx_patch.inc (PCRE2 library separate)
#   - nginx_configure.inc handles PCRE1 vs PCRE2 selection based on version
# CONFIGURE FLAGS:
#   - NGINX_PCRE_TWO='y': --with-pcre=/usr/local/nginx-dep/pcre2-${PCRE2_VER} --with-pcre-jit
#   - NGINX_PCRE_DYNAMIC='y': Uses system libpcre2-8.so instead of static compile
#   - PCRE1 fallback: --with-pcre=/usr/local/nginx-dep/pcre-${PCRE_VER}
#   - PCRE_LDOPT and PCRE_CCOPT linker/compiler flags set based on choice
# OS COMPATIBILITY:
#   - EL7/8/9/10: Full PCRE2 support with modern gcc
#   - EL6: PCRE1 only (PCRE2 requires C++11)
#   - AlmaLinux/Rocky: Identical compatibility to RHEL
#   - Requires: gcc-c++ 4.8+ for PCRE2 compilation
# PERFORMANCE IMPACT:
#   - PCRE2 is ~10-30% faster than PCRE1 for complex regex patterns
#   - Memory: Slightly lower memory usage than PCRE1 (~5-10% reduction)
#   - JIT compilation: --with-pcre-jit provides 2-10x regex speedup
#   - Recommended for high-traffic sites with complex location/rewrite rules
# SECURITY IMPLICATIONS:
#   - PCRE2 receives active security updates (PCRE1 maintenance-only since 2015)
#   - PCRE2 has better protection against ReDoS (regex denial of service) attacks
#   - PCRE1 has known vulnerabilities (CVE-2015-xxxx series)
#   - Recommended: Enable PCRE2 for security-sensitive deployments
# CONFLICTS:
#   - Requires nginx >= 1.21.5 (automatically falls back to PCRE1 otherwise)
#   - Compatible with NGINX_MODSECURITY, ORESTY_LUANGINX (all use PCRE regex)
#   - Check nginx_configure.inc:761,766,772,792 for dynamic vs static PCRE2 selection
#   - grep 'NGINX_PCRE|NGINX_MODSECURITY|ORESTY_LUANGINX' shows compatibility (nginx_upgrade.inc:87,164)
# RECOMMENDATION:
#   - Enable for nginx 1.21.5+ installations for better performance and security
#   - Disable for nginx < 1.21.5 (automatic fallback to PCRE1)
#   - Pair with --with-pcre-jit for maximum regex performance
#   - Monitor upgrade logs: centminmod_*_pcre2_install.log (nginx_upgrade.inc:1013,1143)
# Related: NGINX_PCRE_DYNAMIC, NGINX_MODSECURITY, ORESTY_LUANGINX, PCRE_VER, PCRE2_VER
# Reference: https://www.pcre.org/current/doc/html/pcre2.html
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
# ModSecurity Web Application Firewall (WAF) Module Integration
# Installs ModSecurity v3 (libmodsecurity) nginx connector for advanced web application protection.
# AUTOMATIC BEHAVIOR:
#   - When NGINX_MODSECURITY='y': Downloads and compiles libmodsecurity3 library
#   - Installs ModSecurity-nginx connector module
#   - Downloads OWASP Core Rule Set (CRS) to /usr/local/nginx/conf/modsecurity/
#   - Creates modsecurity.conf with recommended security rules
#   - Requires manual vhost configuration to enable ModSecurity directives
# NGINX VERSION REQUIREMENTS:
#   - Minimum: Nginx 1.11.6+ (ModSecurity v3 nginx connector requirement)
#   - Recommended: Nginx 1.18.0+ for stable ModSecurity integration
#   - Works with all modern nginx versions (1.25.x+)
# UPGRADE BEHAVIORS: (from nginx_upgrade.inc)
#   - Line 66,143: if NGINX_MODSECURITY='y' checks for libmodsecurity3 during upgrade
#   - Line 87,164: grep -E shows NGINX_MODSECURITY config status
#   - Line 995,1124: Echoes NGINX_MODSECURITY=$NGINX_MODSECURITY in debug output
#   - Recompiles ModSecurity connector with new nginx version
# PATCH BEHAVIORS:
#   - No specific ModSecurity patches in nginx_patch.inc
#   - ModSecurity library patches handled separately via libmodsecurity repo
# CONFIGURE FLAGS:
#   - --add-module=../ModSecurity-nginx/ (ModSecurity nginx connector)
#   - Requires: libmodsecurity.so in /usr/local/modsecurity/lib/
#   - Adds: -I/usr/local/modsecurity/include to CFLAGS
#   - Adds: -L/usr/local/modsecurity/lib -lmodsecurity to LDFLAGS
# OS COMPATIBILITY:
#   - EL7: Full support with devtoolset-11+ (C++14 required)
#   - EL8/9/10: Native gcc-11+ support, no devtoolset needed
#   - Requires: gcc-c++, libxml2-devel, libyajl-devel, GeoIP-devel, libcurl-devel
#   - AlmaLinux/Rocky: Identical compatibility to RHEL
# PERFORMANCE IMPACT:
#   - CPU: +20-50% CPU usage per request (rule evaluation overhead)
#   - Memory: +50-150MB per worker (rule set and regex compilation)
#   - Latency: +10-50ms per request (depends on rule set size)
#   - Disk: Audit logs can grow to GB/day if not rotated
#   - Recommended: Use only for high-security applications, consider CDN WAF instead
# SECURITY IMPLICATIONS:
#   - Provides protection against OWASP Top 10 vulnerabilities
#   - Blocks SQL injection, XSS, CSRF, RCE attempts
#   - OWASP CRS provides 200+ security rules (configurable paranoia level)
#   - Can generate false positives requiring rule tuning
#   - Audit logging for compliance (PCI-DSS, HIPAA, etc.)
# CONFLICTS:
#   - May conflict with NGINX_OPENRESTY='y' (check nginx_configure.inc:589)
#   - High CPU/memory usage may conflict with resource-constrained environments
#   - Check nginx_configure.inc:786,806 for PCRE and lua-nginx-module compatibility
# RECOMMENDATION:
#   - Enable for high-security web applications (finance, healthcare, e-commerce)
#   - Start with OWASP CRS paranoia level 1, tune rules based on false positives
#   - Disable for internal APIs, CDN-protected sites, or low-latency requirements
#   - Consider Cloudflare WAF or AWS WAF as managed alternatives
#   - Monitor CPU/memory impact, enable only on critical applications
# Related: NGINX_PCRE_TWO, ORESTY_LUANGINX (alternative Lua-based WAF)
# Reference: https://github.com/SpiderLabs/ModSecurity-nginx
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
# SSL/TLS Handshake Fingerprinting Module Integration
# Adds ability to fingerprint TLS clients based on ClientHello message for bot detection and traffic analysis.
# AUTOMATIC BEHAVIOR:
#   - When NGINX_TLS_FINGERPRINT='y' and OpenSSL 1.1.1+: Applies TLS fingerprinting patches (nginx_patch.inc:623)
#   - Adds $ssl_ja3 and $ssl_ja3_hash nginx variables to all SSL vhosts
#   - Creates JA3 fingerprint hash from TLS ClientHello parameters
#   - Requires OpenSSL 1.1.1+ (SSL_client_hello_get0_* API functions)
# NGINX VERSION REQUIREMENTS:
#   - Minimum: Nginx 1.21.0+ (nginx_patch.inc:623: DETECT_NGXVER >= 1021000)
#   - OpenSSL 1.1.1+ required (DETECTOPENSSL_ONEONE = '1.1.1')
#   - Not compatible with LibreSSL or BoringSSL (OpenSSL-specific API)
# UPGRADE BEHAVIORS: (from nginx_upgrade.inc)
#   - No specific upgrade logic in nginx_upgrade.inc
#   - Patches reapplied during nginx recompilation
# PATCH BEHAVIORS: (from nginx_patch.inc)
#   - Line 623: if NGINX_TLS_FINGERPRINT='y' and nginx >= 1.21.0 and OpenSSL 1.1.1+ applies patches
#   - Patch adds $ssl_ja3 and $ssl_ja3_hash variables to ngx_http_ssl_module
#   - Extracts TLS ClientHello fields: version, ciphers, extensions, curves, point formats
# CONFIGURE FLAGS:
#   - Requires: LIBRESSL_SWITCH='n' and BORINGSSL_SWITCH='n' (nginx_configure.inc:862)
#   - No additional ./configure flags, patch modifies SSL module behavior
# OS COMPATIBILITY:
#   - EL7: OpenSSL 1.0.2 (not supported, requires OpenSSL 1.1.1+)
#   - EL8/9/10: OpenSSL 1.1.1+ or 3.x (full support)
#   - Requires: OpenSSL 1.1.1+ with SSL_client_hello_get0_* API functions
# PERFORMANCE IMPACT:
#   - CPU: +0.1-0.5% per SSL handshake (minimal overhead)
#   - Memory: +100-200 bytes per connection for JA3 hash storage
#   - Latency: No noticeable impact (<0.1ms per handshake)
#   - Negligible performance impact for most deployments
# SECURITY IMPLICATIONS:
#   - Enables bot detection: JA3 fingerprints identify automated tools
#   - Can block scrapers, credential stuffers, DDoS bots
#   - Privacy concern: JA3 fingerprinting can track users across sites
#   - Use cases: Rate limiting by JA3 hash, blocking known bad actors
#   - Example nginx config: if ($ssl_ja3_hash = "bad_bot_hash") { return 444; }
# CONFLICTS:
#   - Incompatible with LIBRESSL_SWITCH='y' (LibreSSL lacks required API)
#   - Incompatible with BORINGSSL_SWITCH='y' (BoringSSL different SSL API)
#   - Requires OpenSSL 1.1.1+ (nginx_configure.inc:862 enforcement)
# RECOMMENDATION:
#   - Enable for high-security sites needing bot detection
#   - Recommended for: API rate limiting, DDoS mitigation, fraud prevention
#   - Disable if: Privacy-focused site, no bot/scraper issues
#   - Pair with logging: log_format with $ssl_ja3_hash for analysis
#   - Use JA3 databases: Match against known bot/scraper fingerprints
# Related: NGINX_VHOSTSSL, OPENSSL_SYSTEM_USE, AWS_LC_SWITCH
# Reference: https://github.com/salesforce/ja3, https://github.com/fooying/nginx-ssl-ja3
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
# HTTP/2 Server Push Module Support (Deprecated)
# Controls HTTP/2 server push functionality for proactively sending resources to clients.
# AUTOMATIC BEHAVIOR:
#   - When NGINX_HTTPPUSH='y' and nginx >= 1.11.12: Applies HTTP/2 server push patches (nginx_patch.inc:585)
#   - Enables http2_push_preload directive in nginx configuration
#   - Allows automatic resource push based on Link: preload headers
#   - Deprecated: HTTP/2 server push removed in Chrome 106+ (2022), Edge 106+, Safari 15+
# NGINX VERSION REQUIREMENTS:
#   - Minimum: Nginx 1.11.12+ (nginx_patch.inc:585: DETECT_NGXVER >= 1011012)
#   - Nginx 1.13.9+: Native http2_push and http2_push_preload support
#   - Modern browsers: HTTP/2 server push deprecated/removed (Chrome 106+, 2022)
# UPGRADE BEHAVIORS:
#   - No specific upgrade logic in nginx_upgrade.inc
#   - Feature deprecated by browsers, patch has no practical effect
# PATCH BEHAVIORS: (from nginx_patch.inc)
#   - Line 585: if NGINX_HTTPPUSH='y' and nginx >= 1.11.12 applies server push patches
#   - Patches for http2_push_preload and server push API changes
# CONFIGURE FLAGS:
#   - No specific ./configure flags (part of --with-http_v2_module)
#   - nginx_configure.inc:854: if NGINX_HTTPPUSH='y' enables push patches
# OS COMPATIBILITY:
#   - EL6/7/8/9/10: Technical compatibility, but feature deprecated by browsers
#   - HTTP/2 server push removed from Chrome 106+, Edge 106+, Safari 15+ (2022)
# PERFORMANCE IMPACT:
#   - HTTP/2 server push had mixed performance results (often slower than traditional approach)
#   - Browsers removed support due to performance issues and complexity
#   - No benefit to enabling this feature on modern browsers
# SECURITY IMPLICATIONS:
#   - HTTP/2 server push had cache poisoning vulnerabilities
#   - Browsers removed support partially due to security concerns
# CONFLICTS:
#   - No conflicts with other nginx modules
#   - Feature ignored by modern browsers regardless of nginx configuration
# RECOMMENDATION:
#   - DO NOT ENABLE - HTTP/2 server push deprecated and removed by browsers
#   - Use <link rel="preload"> in HTML instead (supported by all browsers)
#   - Use Early Hints (HTTP 103) for similar functionality (nginx 1.23.0+)
#   - Disable for all new installations
# Related: NGINX_HTTP2, NGINX_HTTP3, NGINX_QUIC_SUPPORT
# Reference: https://developer.chrome.com/blog/removing-push/ (Chrome removed in 2022)
# Git History: Added 2017-06-07 (5fa91cb6)
#NGINX_HTTPPUSH='n'  

# Nginx HTTP/2 HPACK Full Encoding
# Maximum allowed Nginx version for HPACK patch
# Git History: Added 2022-01-27 (6914d664)
#NGINX_HPACK_ALLOWED_VER='1024005'  

# Nginx Dynamic TLS Records
# Optimize TLS over TCP to reduce latency
# Dynamic TLS Records Patch for HTTP/2 Performance Optimization
# Adjusts TLS record size dynamically to reduce latency for small HTTP/2 responses while maintaining throughput for large transfers.
# AUTOMATIC BEHAVIOR:
#   - When NGINX_DYNAMICTLS='y': Applies dynamic TLS record patches to nginx source (nginx_patch.inc:550,1213)
#   - Starts with 1 TLS record (~4KB) for first packets (low latency)
#   - Gradually increases to 4 records (~16KB) for subsequent packets (high throughput)
#   - Reduces initial response time by 25-40% for HTTP/2 connections
#   - Automatically disabled if NGINXPATCH='n' or NGINX_HTTP2='n' (nginx_upgrade.inc:794)
# NGINX VERSION REQUIREMENTS:
#   - Supported: Nginx 1.21.0 through ~1.25.x (nginx_patch.inc:1248,1298: NGINX_DYNAMICTLS_ALLOWED_VER)
#   - Nginx >= 1.21.0 and <= NGINX_DYNAMICTLS_ALLOWED_VER (version ceiling check)
#   - Automatically disabled for versions outside supported range
#   - Requires: HTTP/2 enabled (--with-http_v2_module)
# UPGRADE BEHAVIORS: (from nginx_upgrade.inc)
#   - Line 794: if NGINXPATCH='n' or NGINX_HTTP2='n' or NGINX_DYNAMICTLS='n' skips dynamic TLS patches
#   - Preserves dynamic TLS settings during nginx upgrades within supported version range
# PATCH BEHAVIORS: (from nginx_patch.inc)
#   - Line 550: Sets NGINX_DYNAMICTLS='y' automatically when conditions met
#   - Line 1211: Forces NGINX_DYNAMICTLS='n' if outside supported version range
#   - Line 1213: if NGINX_DYNAMICTLS='y' applies TLS record size adjustment patches
#   - Line 1248,1298: elif nginx >= 1.21.0 and nginx <= NGINX_DYNAMICTLS_ALLOWED_VER applies patches
#   - Patch modifies ngx_http_v2_filter_module.c TLS record buffer logic
# CONFIGURE FLAGS:
#   - No specific ./configure flags (patch modifies runtime TLS behavior)
#   - Requires: --with-http_v2_module (HTTP/2 support)
#   - Requires: --with-http_ssl_module (SSL/TLS support)
# OS COMPATIBILITY:
#   - EL6/7/8/9/10: Full compatibility if nginx version in supported range
#   - Works with OpenSSL, BoringSSL, LibreSSL, AWS-LC
#   - No additional dependencies beyond standard SSL library
# PERFORMANCE IMPACT:
#   - Latency: -25-40% TTFB (Time To First Byte) for HTTP/2 small responses
#   - Throughput: Maintained via adaptive TLS record sizing
#   - CPU: Negligible overhead (<1% CPU for TLS record adjustment)
#   - Memory: No additional memory usage
#   - Best for: HTTP/2 APIs, SSR (Server-Side Rendering) apps, low-latency requirements
# SECURITY IMPLICATIONS:
#   - No security implications (TLS record size is transparent to encryption)
#   - Smaller records reduce BEAST attack surface (theoretical, already mitigated)
#   - Does not affect TLS version or cipher suite selection
# CONFLICTS:
#   - Requires NGINX_HTTP2='y' (HTTP/2 enabled)
#   - Disabled if NGINXPATCH='n' (nginx_upgrade.inc:794)
#   - Disabled if nginx version outside NGINX_DYNAMICTLS_ALLOWED_VER range
# RECOMMENDATION:
#   - Enable for HTTP/2-enabled sites prioritizing low latency
#   - Recommended for: API servers, real-time apps, mobile-optimized sites
#   - Disable if: Not using HTTP/2, prioritize throughput over latency
#   - Only effective with HTTP/2, no benefit for HTTP/1.1
#   - Monitor nginx version compatibility (may break on new nginx releases)
# Related: NGINX_HTTP2, NGINX_HTTP3, NGINX_QUIC_SUPPORT, NGINXPATCH
# Reference: https://blog.cloudflare.com/optimizing-tls-over-tcp-to-reduce-latency/
# Git History: Added 2016-06-11 (cfcc21e7)
#NGINX_DYNAMICTLS='n'  
# Git History: Added 2022-01-27 (6914d664)
#NGINX_DYNAMICTLS_ALLOWED_VER='1024005'  

# HTTP/3 QUIC Protocol Support for Nginx
# Enable HTTP/3 over QUIC protocol using nginx-quic branch with quicTLS OpenSSL fork.
# AUTOMATIC BEHAVIOR:
#   - Auto-sets OPENSSL_SYSTEM_USE='n' (line 482-484, 614) - requires custom quicTLS
#   - Auto-opens UDP 443 in CSF firewall (line 518-547) - UDP_IN, UDP_OUT, UDP6_IN, UDP6_OUT
#   - Auto-disabled when AWS_LC_SWITCH='y' (line 493-496) - AWS-LC has priority for QUIC
#   - Auto-disables NGINX_HPACK when NGINX_QUIC_HPACK_SUPPORT!='y' (line 631-634)
#   - Requires quicTLS OpenSSL 1.1.1 fork compilation
# NGINX VERSION REQUIREMENTS:
#   - Nginx 1.25.0+ for stable QUIC support (line 614, 1107, 1213)
#   - nginx-quic branch: Development QUIC builds
#   - DETECT_NGXVER >= 1025000 required
# OPENSSL REQUIREMENTS:
#   - quicTLS fork (OpenSSL 1.1.1+quic patches) required
#   - Location: /opt/openssl-quic/ or ../openssl-quic-nginx/
#   - QUIC RPM packages: centmin-openssl11-quic-compat (line 615-623)
# OS COMPATIBILITY:
#   - EL7/8/9/10: Supported with quicTLS RPMs or source compilation
#   - Requires kernel UDP stack support (3.10+ kernel)
# CONFIGURE FLAGS:
#   - --with-http_v3_module (Nginx 1.25+)
#   - --with-stream_quic_module (for QUIC stream support)
#   - --with-openssl=/opt/openssl-quic
#   - --with-cc-opt: QUICTLS_CC_OPT='-I/opt/openssl-quic/include'
#   - --with-ld-opt: QUICTLS_LD_OPT='-L/opt/openssl-quic/lib -lssl -lcrypto'
# QUIC TLS RPM PACKAGES (NGINX_QUIC_RPMS='y'):
#   - EL7: centmin-openssl11-quic-compat-1.1.1q-1.el7.x86_64.rpm
#   - EL8: centmin-openssl11-quic-compat-1.1.1q-1.el8.x86_64.rpm
#   - EL9: centmin-openssl11-quic-compat-1.1.1q-1.el9.x86_64.rpm
#   - EL10: centmin-openssl11-quic-compat-1.1.1q-1.el10.x86_64.rpm
# CSF FIREWALL CONFIGURATION:
#   - Auto-adds UDP 443 to UDP_IN, UDP_OUT (line 519-532)
#   - Auto-adds UDP 443 to UDP6_IN, UDP6_OUT (line 533-546)
#   - Creates CSF profile backups before modifications
# CONFLICTS:
#   - Incompatible with OPENSSL_SYSTEM_USE='y' (requires custom quicTLS)
#   - Incompatible with LIBRESSL_SWITCH='y' (LibreSSL lacks QUIC)
#   - Disabled when AWS_LC_SWITCH='y' (AWS-LC QUIC takes priority)
#   - Disabled when BORINGSSL_SWITCH='y' (use BoringSSL QUIC instead)
# HTTP/3 FEATURES:
#   - QUIC transport protocol (UDP-based)
#   - 0-RTT connection resumption
#   - Improved congestion control vs TCP
#   - Connection migration (IP/port changes)
#   - Multiplexed streams without head-of-line blocking
# BROWSER SUPPORT:
#   - Chrome 87+ (2020): Full HTTP/3 support
#   - Firefox 88+ (2021): Full HTTP/3 support
#   - Safari 14+ (2021): Full HTTP/3 support
#   - Edge 87+ (2020): Full HTTP/3 support
# PERFORMANCE IMPACT:
#   - Compilation: 2-3x longer (quicTLS + QUIC module)
#   - Runtime: 10-30% faster page loads on high-latency networks
#   - CPU usage: 5-15% higher than HTTP/2 (QUIC crypto overhead)
#   - Best performance gains: Mobile networks, long-distance connections
# USE CASES:
#   - Modern web applications (Chrome/Firefox/Safari users)
#   - Mobile-first websites (QUIC excels on cellular)
#   - Global CDN deployments (connection migration benefits)
#   - Low-latency requirements (0-RTT resumption)
# RECOMMENDATION: Enable for production on Nginx 1.25+ with modern client base.
#   HTTP/3 provides significant performance improvements for mobile and high-latency
#   users. Ensure UDP 443 firewall rules and quicTLS are properly configured.
# Related: AWS_LC_SWITCH, NGINX_QUIC_HPACK_SUPPORT, NGINX_QUIC_RPMS, OPENSSL_SYSTEM_USE
# Reference: https://nginx.org/en/docs/http/ngx_http_v3_module.html
# Git History: Added 2024-06-20 (1cb19e7b)
#NGINX_QUIC_SUPPORT='n'

# Nginx kTLS Support
# Kernel TLS offload (requires OpenSSL 3.0+ and Linux Kernel 5.2+)
# Git History: Added 2022-01-24 (5820ffcf)
#NGINX_KTLS='n'  

# zlib-ng Next-Generation Compression Library
# Replace standard zlib with zlib-ng for superior compression performance and modern CPU optimizations.
# AUTOMATIC BEHAVIOR:
#   - Auto-disables CLOUDFLARE_ZLIB='n' (line 713) - zlib-ng takes priority over Cloudflare zlib
#   - Auto-disables NGINX_ZLIBCUSTOM='n' (line 714)
#   - Requires x86_64 architecture (line 710)
#   - Calls zlibng_install() function for source compilation
# NGINX VERSION COMPATIBILITY:
#   - All Nginx versions compatible
#   - Works with HTTP/2 gzip compression
# OS COMPATIBILITY:
#   - x86_64 required (line 710 check)
#   - EL6/7/8/9/10: All supported
#   - ARM64: Potential future support (not in current code)
# CONFIGURE FLAGS:
#   - --with-zlib=../zlib-ng
#   - Uses ZLIBNG_OPT=' --with-zlib=../zlib-ng' (line 712)
# CONFLICTS:
#   - Mutually exclusive with CLOUDFLARE_ZLIB='y'
#   - Mutually exclusive with NGINX_ZLIBCUSTOM (custom zlib)
# PERFORMANCE IMPACT:
#   - Compression speed: 10-50% faster than zlib 1.2.x (varies by data type)
#   - Decompression speed: 5-20% faster
#   - AVX2/AVX-512/NEON optimizations on modern CPUs
#   - Best performance on Intel Ice Lake+ / AMD Zen3+ / ARM Neoverse
# CPU OPTIMIZATIONS:
#   - x86-64: SSE2, SSSE3, AVX2, AVX-512
#   - ARM: NEON, CRC32 (future support)
#   - Runtime CPU detection and optimal path selection
# COMPRESSION FEATURES:
#   - Maintains full zlib 1.2.x API compatibility
#   - Improved compression ratios (0.1-3% better depending on data)
#   - Modern algorithms (slide_hash, compare256)
# USE CASES:
#   - Modern CPU deployments (2015+ Intel, 2019+ AMD)
#   - High-throughput HTTP/2 gzip compression
#   - Replacing Cloudflare zlib for broader CPU support
# RECOMMENDATION: Enable on modern x86_64 servers for best-in-class gzip performance.
#   Preferred over CLOUDFLARE_ZLIB due to broader CPU optimization support and
#   active maintenance.
# Related: CLOUDFLARE_ZLIB, NGINX_ZLIBCUSTOM
# Reference: https://github.com/zlib-ng/zlib-ng
# Git History: Added 2016-07-29 (bf22a708)
#NGINX_ZLIBNG='n'  

################################################################################
# 14. NGINX OPERATIONAL SETTINGS
################################################################################

# Nginx vs FreeNginx Installation Control
# Controls whether standard Nginx or FreeNginx is used during installation and upgrades.
# AUTOMATIC BEHAVIOR:
#   - When NGINX_INSTALL='y': Downloads from nginx.org official releases (nginx_upgrade.inc:597,612,640,674)
#   - Automatically switches source paths based on NGINX_INSTALL vs FREENGINX_INSTALL detection
#   - Sets download URLs: NGINX_INSTALL sources from nginx.org (nginx_install.inc:113,139,260,466)
#   - Mutual exclusivity check: if [[ "$NGINX_INSTALL" = [yY] || "$FREENGINX_INSTALL" = [yY] ]] (nginx_install.inc:113)
# NGINX VERSION REQUIREMENTS:
#   - Works with all Nginx versions from 1.x to current
#   - Version detection via DETECT_NGXVER numeric comparison throughout codebase
#   - Compatible with Nginx mainline and stable branches
# UPGRADE BEHAVIORS: (from nginx_upgrade.inc)
#   - Line 595-597: if [[ "$FREENGINX_INSTALL" = [yY] ]] then use FreeNginx else use Nginx
#   - Line 609-612: Sets NGINX_LINKFILE variable conditionally for upgrade path detection
#   - Line 638-640: Controls tarball extraction directory naming
#   - Line 671-674: Manages configure script location based on install type
#   - Preserves existing nginx.conf during upgrades regardless of install type
# PATCH BEHAVIORS: (from nginx_patch.inc)
#   - Line 1088: FREENGINX_BACKPORT_PATCHES only applies when NGINX_INSTALL='y' and FREENGINX_INSTALL!='y'
#   - Conditional patch application: standard Nginx gets different patches than FreeNginx
#   - Version-specific patches selected based on NGINX_INSTALL vs FREENGINX_INSTALL state
# CONFIGURE FLAGS:
#   - No direct configure flags, but changes source directory paths used in configure
#   - Affects which tarball is extracted: nginx-${ngver} vs freenginx-${ngver}
# OS COMPATIBILITY:
#   - EL6/7/8/9/10: Full compatibility across all Enterprise Linux versions
#   - Works identically on AlmaLinux, Rocky Linux, Oracle Linux, VzLinux, Circle, Navy, Euro
# PERFORMANCE IMPACT:
#   - No runtime performance difference
#   - Only affects source download location and version management
# SECURITY IMPLICATIONS:
#   - Nginx.org vs FreeNginx.org security update cadence differs
#   - FreeNginx may include patches backported from mainline faster
#   - FREENGINX_BACKPORT_PATCHES='y' can apply FreeNginx patches to standard Nginx builds
# CONFLICTS:
#   - Mutually exclusive with FREENGINX_INSTALL='y'
#   - Only one can be 'y' at a time, enforced by conditional logic
#   - Cannot have both enabled simultaneously
# RECOMMENDATION:
#   - Use NGINX_INSTALL='y' for official Nginx.org releases (default, well-tested)
#   - Use FREENGINX_INSTALL='y' for FreeNginx fork (community-driven, faster patches)
#   - Enable FREENGINX_BACKPORT_PATCHES='y' if using Nginx but want FreeNginx patches
# Related: FREENGINX_INSTALL, FREENGINX_BACKPORT_PATCHES, DETECT_NGXVER
# Reference: https://nginx.org/ vs https://freenginx.org/
# Git History: Original variable, present since early Centmin Mod versions
#NGINX_INSTALL='y'  

# Maintenance Mode Control During Nginx Upgrades
# Controls whether maintenance mode is enabled during nginx upgrades to display maintenance page to users.
# AUTOMATIC BEHAVIOR:
#   - When NGINX_UPDATEMAINTENANCE='y': Creates/enables maintenance mode before nginx upgrade (nginx_upgrade.inc:7)
#   - Displays maintenance.html page from /usr/local/nginx/html/ during upgrade
#   - Automatically disables maintenance mode after successful upgrade completion (nginx_upgrade.inc:20)
#   - Preserves user connections during upgrade (graceful restart)
# NGINX VERSION REQUIREMENTS:
#   - Works with all nginx versions (maintenance mode independent of nginx version)
#   - Uses HTTP 503 Service Unavailable response during maintenance
# UPGRADE BEHAVIORS: (from nginx_upgrade.inc)
#   - Line 3,7: if NGINX_UPDATEMAINTENANCE='y' enters maintenance mode before upgrade
#   - Line 16,20: if NGINX_UPDATEMAINTENANCE='y' exits maintenance mode after upgrade
#   - Maintenance page: /usr/local/nginx/html/maintenance.html (customizable)
#   - nginx reload used for zero-downtime upgrades
# PATCH BEHAVIORS:
#   - No patches in nginx_patch.inc (maintenance mode is vhost-level configuration)
# VHOST BEHAVIORS:
#   - Maintenance mode adds temporary location block returning 503 to all vhosts
#   - Bypasses maintenance for specific IPs (allow localhost, admin IPs)
#   - Example: if (-f /usr/local/nginx/html/maintenance.html) { return 503; }
# CONFIGURE FLAGS:
#   - No ./configure flags affected (runtime configuration only)
# OS COMPATIBILITY:
#   - EL6/7/8/9/10: Full compatibility (bash script-based maintenance mode)
#   - Works with systemd and init.d service management
# PERFORMANCE IMPACT:
#   - Minimal: Only active during upgrade window (typically 5-30 seconds)
#   - 503 responses are fast (no backend processing)
#   - Recommended for production sites to avoid user-facing errors
# SECURITY IMPLICATIONS:
#   - Prevents partial upgrade states from exposing errors to users
#   - Maintenance page should not reveal sensitive system information
#   - Allow admin IPs to bypass maintenance mode for testing
# CONFLICTS:
#   - No conflicts with other nginx modules
#   - Works with all upgrade paths (minor, major version upgrades)
# RECOMMENDATION:
#   - Enable for production sites with active users during upgrades
#   - Customize /usr/local/nginx/html/maintenance.html with branding
#   - Schedule upgrades during low-traffic windows when possible
#   - Disable for development/staging environments (faster upgrades)
# Related: nginx service management, upgrade procedures
# Reference: https://community.centminmod.com/posts/26485/
# Git History: Added with nginx upgrade workflow improvements ~2015-2016
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

# Gzip Compression for Multiple HTTP Status Codes
# Enables gzip compression for HTTP responses beyond just 200 OK (includes 301, 302, 307, 308 redirects).
# AUTOMATIC BEHAVIOR:
#   - When NGINX_GZIP_MULTI_STATUS='y' and nginx >= 1.25.3: Applies gzip status code patch (nginx_patch.inc:172)
#   - Modifies ngx_http_gzip_filter_module to compress redirects and other status codes
#   - Default nginx behavior: Only compresses HTTP 200 responses
#   - Patch adds: 301, 302, 307, 308 redirects to gzip compression candidates
# NGINX VERSION REQUIREMENTS:
#   - Minimum: Nginx 1.25.3+ (nginx_patch.inc:172: DETECT_NGXVER >= 1025003)
#   - Only effective on nginx 1.25.3 and newer versions
#   - Older nginx: gzip_types directive does not affect redirect status codes
# UPGRADE BEHAVIORS:
#   - No specific upgrade logic in nginx_upgrade.inc
#   - Patch automatically applied during nginx compilation if version matches
# PATCH BEHAVIORS: (from nginx_patch.inc)
#   - Line 172: if NGINX_GZIP_MULTI_STATUS='y' and nginx >= 1.25.3 applies patch
#   - Modifies ngx_http_gzip_filter_module.c to expand gzip_types status code list
#   - Patch file: patches/nginx/nginx-gzip-multi-status.patch
# CONFIGURE FLAGS:
#   - No specific ./configure flags (patch modifies runtime gzip behavior)
#   - Requires: --with-http_gzip_static_module for gzip support
# OS COMPATIBILITY:
#   - EL6/7/8/9/10: Full compatibility if nginx >= 1.25.3 installed
#   - No additional dependencies (gzip support in nginx core)
# PERFORMANCE IMPACT:
#   - Minimal: Redirects typically have small response bodies (<1KB)
#   - Gzip compression: ~10-20% CPU overhead for compressing redirect Location headers
#   - Bandwidth savings: Negligible for most sites (redirects are small)
#   - Recommended: Enable for API servers with large redirect response bodies
# SECURITY IMPLICATIONS:
#   - No direct security implications (gzip compression on redirects)
#   - BREACH attack not applicable to redirects (no sensitive data in redirect body)
# CONFLICTS:
#   - No conflicts with other nginx modules
#   - Compatible with all gzip configurations (gzip_types, gzip_min_length, etc.)
# RECOMMENDATION:
#   - Enable for sites with complex redirect chains (multi-stage redirects)
#   - Recommended for: API gateways with redirect responses containing data
#   - Disable for: Most standard websites (minimal benefit, adds complexity)
#   - Only effective on nginx 1.25.3+, no effect on older versions
# Related: NGINX_ZLIBNG, NGINX_LIBBROTLI, NGINX_ZSTD, gzip configuration
# Reference: Nginx gzip module documentation
# Git History: Added 2024-01-05 (fd3b9c91)
#NGINX_GZIP_MULTI_STATUS='n'  

# Nginx Patch Enable
# Nginx Source Code Patching Control
# Master switch controlling whether ANY patches are applied to nginx source code before compilation.
# AUTOMATIC BEHAVIOR:
#   - When NGINXPATCH='y' and FPM_NGINX_RPM!='y': Applies all enabled patches to nginx source (nginx_install.inc:275)
#   - Triggers NGINXPATCH_DELAY second delay before patching (visual indication)
#   - When NGINXPATCH='n': Completely skips all nginx patch application (nginx_upgrade.inc:794-795)
#   - Disables SPDY patches, dynamic TLS patches, security patches, compatibility patches
#   - Controls patch application via nginxpatch() function in nginx_patch.inc
# NGINX VERSION REQUIREMENTS:
#   - Works with all nginx versions (master control for patch system)
#   - Individual patches have version requirements checked independently
# UPGRADE BEHAVIORS: (from nginx_upgrade.inc)
#   - Line 794-795: if NGINXPATCH='n' or NGINX_HTTP2='n' or NGINX_DYNAMICTLS='n' skips all patches
#   - Line 953: if NGINXPATCH='y' displays "Nginx Patch Time" message with NGINXPATCH_DELAY
#   - Line 955: cecho "Nginx Patch Time - $NGINXPATCH_DELAY seconds delay"
#   - Patches applied after nginx source extraction, before ./configure
# PATCH BEHAVIORS: (from nginx_patch.inc)
#   - Line 1105: sleep "$NGINXPATCH_DELAY" (default 5 seconds visual delay)
#   - Master control: NGINXPATCH='n' disables ALL patches regardless of individual patch variables
#   - Patch categories affected: Security, compatibility, SPDY, HTTP/2, TLS, module-specific
# CONFIGURE FLAGS:
#   - No specific ./configure flags (controls pre-configure patching stage)
# OS COMPATIBILITY:
#   - EL6/7/8/9/10: Full compatibility (patch command standard across all systems)
#   - Requires: patch utility (installed by default)
# PERFORMANCE IMPACT:
#   - NGINXPATCH_DELAY adds 5 seconds to build time (visual indicator only)
#   - Patches themselves have varied performance impacts (see individual patch variables)
# SECURITY IMPLICATIONS:
#   - NGINXPATCH='n': Disables security patches (not recommended for production)
#   - NGINXPATCH='y': Enables security patches (CVE fixes, vulnerability mitigations)
#   - Critical: Keep NGINXPATCH='y' unless debugging build issues
# CONFLICTS:
#   - NGINXPATCH='n' disables: NGINX_DYNAMICTLS, NGINX_SPDYPATCHED, FREENGINX_BACKPORT_PATCHES, etc.
#   - May need to set NGINXPATCH='n' temporarily when debugging compilation failures
#   - Check nginx_upgrade.inc:794 for patch dependencies
# RECOMMENDATION:
#   - Keep NGINXPATCH='y' for all production deployments (enables security patches)
#   - Set NGINXPATCH='n' only for: Debugging build failures, testing upstream nginx bugs
#   - NGINXPATCH_DELAY can be reduced to 0 in custom_config.inc for faster builds
#   - Monitor logs: centminmod_*_nginx_patch.log shows applied patches
# Related: NGINX_DYNAMICTLS, NGINX_SPDYPATCHED, FREENGINX_BACKPORT_PATCHES, NGINXPATCH_DELAY
# Reference: Centmin Mod patching system documentation
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

# OCSP Stapling Cache TTL Override Control
# Allows manual override of OCSP stapling cache time-to-live value in nginx SSL configuration.
# AUTOMATIC BEHAVIOR:
#   - When NGINX_STAPLE_CACHE_OVERRIDE='y': Uses NGINX_STAPLE_CACHE_TTL value instead of default (nginx_upgrade.inc:1504-1505)
#   - Applies to all SSL vhosts created after enabling this variable
#   - Modifies nginx ssl_stapling_verify and ssl_stapling directives in vhost templates
#   - Default OCSP stapling cache: 10m (10 minutes), can be overridden with custom value
# NGINX VERSION REQUIREMENTS:
#   - Minimum: Nginx 1.3.7+ (ssl_stapling directive introduced)
#   - Recommended: Nginx 1.19.0+ (improved OCSP stapling resolver handling)
#   - All modern nginx versions support OCSP stapling configuration
# UPGRADE BEHAVIORS: (from nginx_upgrade.inc)
#   - Line 1504-1505: if NGINX_STAPLE_CACHE_OVERRIDE='y' displays detection message with NGINX_STAPLE_CACHE_TTL value
#   - Preserves custom OCSP cache TTL settings during nginx upgrades
#   - Does not modify existing vhost configs, only affects new vhost creation
# PATCH BEHAVIORS: (from nginx_patch.inc)
#   - Line 205: Checked during patch application to determine OCSP cache configuration
#   - No direct patches applied, but affects SSL configuration template generation
# VHOST BEHAVIORS: (from nginx_addvhost.inc)
#   - Applied during SSL vhost creation when NGINX_VHOSTSSL='y'
#   - Sets ssl_stapling_verify on|off based on NGINX_STAPLE_CACHE_TTL
#   - Configures resolver directive for OCSP validation
#   - Example nginx config: ssl_stapling_verify on; ssl_stapling_cache shared:staple_cache:${NGINX_STAPLE_CACHE_TTL};
# CONFIGURE FLAGS:
#   - No nginx ./configure flags affected
#   - Runtime nginx.conf directive: ssl_stapling_cache shared:staple_cache:10m (or custom value)
# OS COMPATIBILITY:
#   - EL6/7/8/9/10: Full compatibility across all Enterprise Linux versions
#   - Requires system resolver (DNS) for OCSP validation
#   - Works with all SSL libraries: OpenSSL, LibreSSL, BoringSSL, AWS-LC
# PERFORMANCE IMPACT:
#   - Larger cache TTL reduces OCSP validation requests to Certificate Authority
#   - Recommended: 10m-1h for most sites (balance freshness vs CA load)
#   - High-traffic sites: Consider 30m-1h to reduce upstream OCSP requests
#   - Memory: ~1KB per cached OCSP response, negligible for 10-100 vhosts
# SECURITY IMPLICATIONS:
#   - OCSP stapling improves SSL handshake performance and privacy
#   - Longer TTL means delayed detection of certificate revocation (rare)
#   - Recommended TTL: 5m-1h (balances security freshness with performance)
#   - OCSP responses signed by CA, cache tampering not a major concern
# CONFLICTS:
#   - No conflicts with other variables
#   - Works alongside all SSL library choices (OpenSSL, BoringSSL, LibreSSL, AWS-LC)
#   - Compatible with Let's Encrypt, commercial CAs, and self-signed certificates
# RECOMMENDATION:
#   - Enable with NGINX_STAPLE_CACHE_TTL='30m' for balanced performance/security
#   - High-traffic sites: Use '1h' to reduce CA load
#   - Security-critical sites: Use '5m' for faster revocation detection
#   - Most sites: Keep default '10m' for good balance
# Related: NGINX_VHOSTSSL, LETSENCRYPT_DETECT, SSL configuration variables
# Reference: https://nginx.org/en/docs/http/ngx_http_ssl_module.html#ssl_stapling
# Git History: Added with OCSP stapling support ~2015-2016
#NGINX_STAPLE_CACHE_OVERRIDE='n'  

# Nginx OCSP Stapling Cache TTL
# OCSP stapling cache refresh time in seconds
# Default: 86400 (24 hours)
# Git History: Added 2020-04-15 (598da664)
#NGINX_STAPLE_CACHE_TTL='86400'  

# Nginx Vhost SSL
# SSL/TLS Virtual Host Creation Support
# Controls whether SSL/TLS configuration is enabled during nginx virtual host creation via nv (nginx vhost) wizard.
# AUTOMATIC BEHAVIOR:
#   - When NGINX_VHOSTSSL='y': Enables SSL certificate options during nv vhost creation wizard (nginx_addvhost.inc:691)
#   - Automatically creates port 443 SSL listener in vhost configuration
#   - Generates self-signed SSL certificate if no certificate specified
#   - Configures modern SSL/TLS settings: TLS 1.2+, strong cipher suites, OCSP stapling
#   - Creates /usr/local/nginx/conf/ssl/${domain}/ directory for certificates
# NGINX VERSION REQUIREMENTS:
#   - Requires: Nginx compiled with --with-http_ssl_module
#   - Minimum: Nginx 1.0.0+ (basic SSL support)
#   - Recommended: Nginx 1.19.0+ for TLS 1.3 and modern SSL features
# UPGRADE BEHAVIORS:
#   - No direct upgrade logic in nginx_upgrade.inc
#   - Preserves existing SSL vhost configurations during nginx upgrades
# PATCH BEHAVIORS:
#   - No specific SSL patches in nginx_patch.inc (SSL handled by OpenSSL library)
# VHOST BEHAVIORS: (from nginx_addvhost.inc)
#   - Line 691: if NGINX_VHOSTSSL='y' enables SSL vhost creation workflow
#   - Line 1588: Complex conditional checks for SSL certificate types (self-signed, Let's Encrypt, etc.)
#   - Supported certificate types: self-signed ('y'), Let's Encrypt ('le', 'led', 'lelive', 'lelived')
#   - SSL vhost template: Creates separate HTTP (port 80) and HTTPS (port 443) server blocks
#   - Automatic HTTP->HTTPS redirect configuration (optional)
#   - OCSP stapling, HSTS, SSL session caching automatically configured
# CONFIGURE FLAGS:
#   - Requires: --with-http_ssl_module in nginx ./configure
#   - SSL library options: OpenSSL, BoringSSL, LibreSSL, AWS-LC (controlled by other variables)
# OS COMPATIBILITY:
#   - EL6/7/8/9/10: Full compatibility across all Enterprise Linux versions
#   - Requires: OpenSSL 1.0.1+ (TLS 1.2 support), OpenSSL 1.1.1+ (TLS 1.3 support)
#   - AlmaLinux/Rocky: Identical compatibility to RHEL
# PERFORMANCE IMPACT:
#   - SSL handshake: 1-5ms per connection (depends on key size, TLS version)
#   - CPU: ~10-20% CPU usage for SSL handshakes on busy sites
#   - Memory: ~50KB per SSL connection for session cache
#   - Recommended: Use TLS 1.3, ECDSA certificates, hardware AES-NI for best performance
# SECURITY IMPLICATIONS:
#   - Enables HTTPS for encrypted client-server communication
#   - Modern SSL configuration: TLS 1.2+, disables weak ciphers (RC4, 3DES, MD5)
#   - OCSP stapling improves privacy (clients don't contact CA)
#   - HSTS (HTTP Strict Transport Security) prevents downgrade attacks
#   - Self-signed certificates: Browser warnings, not suitable for production
#   - Let's Encrypt certificates: Browser-trusted, free, auto-renewal
# CONFLICTS:
#   - No conflicts with other nginx modules
#   - Compatible with LETSENCRYPT_DETECT='y' for free SSL certificates
#   - Works with VHOST_CFAUTHORIGINPULL='y' for Cloudflare origin certificates
# RECOMMENDATION:
#   - Enable for all production websites (HTTPS is now standard practice)
#   - Use Let's Encrypt for free, trusted SSL certificates (LETSENCRYPT_DETECT='y')
#   - Self-signed certificates: Development/testing only, not production
#   - Pair with HSTS, OCSP stapling, TLS 1.3 for optimal security
# Related: LETSENCRYPT_DETECT, VHOST_CFAUTHORIGINPULL, SELFSIGNEDSSL_ECDSA, NGINX_STAPLE_CACHE_OVERRIDE
# Reference: https://nginx.org/en/docs/http/ngx_http_ssl_module.html
# Git History: Added with SSL vhost support ~2015
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

# OpenResty Nginx Lua Modules and Components Integration
# Controls installation of OpenResty-derived Lua modules including lua-nginx-module, ngx_devel_kit, and related components.
# AUTOMATIC BEHAVIOR:
#   - When NGINX_OPENRESTY='y': Automatically sets ORESTY_LUANGINX='y' (nginx_configure.inc:1652)
#   - Auto-detects redis-nginx-level-cache and forces NGINX_OPENRESTY='y' if detected (nginx_configure.inc:664)
#   - Downloads lua-nginx-module, ngx_devel_kit, set-misc-nginx, echo-nginx modules
#   - Installs LuaJIT runtime and required Lua libraries
#   - Creates /usr/local/lib/lua/5.1/ directory structure for Lua modules
# NGINX VERSION REQUIREMENTS:
#   - Minimum: Nginx 1.11.6+ for most OpenResty modules (nginx_patch.inc:977)
#   - Nginx 1.27.0-1.27.x with ORESTY_LUANGINX requires specific patches (nginx_patch.inc:5)
#   - Nginx 1.11.11+ for lua-nginx-module 0.10.7/0.10.8 compatibility (nginx_patch.inc:856)
# UPGRADE BEHAVIORS: (from nginx_upgrade.inc)
#   - Line 494: Skips ngx_pagespeed module check if NGINX_OPENRESTY='y'
#   - Preserves existing Lua module configurations during upgrades
#   - Recompiles all Lua modules to match new Nginx version
# PATCH BEHAVIORS: (from nginx_patch.inc)
#   - Line 5: Nginx 1.27.0-1.28.0 + ORESTY_LUANGINX='y' gets lua-nginx-module compatibility patches
#   - Line 944: Nginx 1.11.11+ with echo 0.60 gets echo module patches
#   - Line 977: Nginx 1.11.6+ gets OpenResty component compatibility patches
#   - Automatic patch application for ngx_devel_kit and lua-nginx-module version mismatches
# VHOST BEHAVIORS:
#   - Does not directly affect vhost creation
#   - Enables Lua scripting capabilities in all vhosts automatically
#   - Allows access_by_lua, content_by_lua directives in vhost configs
# CONFIGURE FLAGS:
#   - --add-module=../lua-nginx-module-${ORESTY_LUANGINXVER}
#   - --add-module=../ngx_devel_kit-${NGX_DEVEL_VER}
#   - --add-module=../set-misc-nginx-module-${SETMISCVER}
#   - --add-module=../echo-nginx-module-${ECHOVER}
#   - Adds LUALD_OPT and LUAJIT_XCFLAGOPT linker flags
# OS COMPATIBILITY:
#   - EL6: LuaJIT 2.0.x, OpenResty components 2015-2016 era
#   - EL7/8/9/10: LuaJIT 2.1.x, Modern OpenResty components
#   - AlmaLinux/Rocky: Full compatibility with all LuaJIT versions
# PERFORMANCE IMPACT:
#   - Adds ~2-5MB to nginx binary size
#   - LuaJIT provides near-native C performance for Lua code
#   - Minimal overhead if Lua directives not actively used
#   - Can replace PHP for simple tasks with better performance
# SECURITY IMPLICATIONS:
#   - Lua scripting adds attack surface if not properly sandboxed
#   - lua-resty-core provides security-hardened Lua functions
#   - Enables advanced rate limiting and WAF capabilities via Lua
# CONFLICTS:
#   - Not compatible with NGINX_PAGESPEED='y' in some configurations
#   - May conflict with ModSecurity module in certain scenarios
#   - Requires PCRE or PCRE2 for regex support in Lua
# RECOMMENDATION:
#   - Enable for advanced caching scenarios (redis-nginx-level-cache)
#   - Recommended for API gateways and dynamic content manipulation
#   - Disable if only serving static files or PHP applications
#   - Enable for WAF capabilities with lua-resty-waf
# Related: ORESTY_LUANGINX, ORESTY_LUANGINXVER, NGX_DEVEL_VER, SETMISCVER, ECHOVER
# Reference: https://openresty.org/en/, https://github.com/openresty/lua-nginx-module
# Git History: Added with OpenResty integration ~2014-2015
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
# Nginx Sticky Sessions Module Integration (Session Persistence)
# Installs nginx-sticky-module for cookie-based session persistence (load balancer session affinity).
# AUTOMATIC BEHAVIOR:
#   - When NGINX_STICKY='y': Downloads nginx-sticky-module from GitHub master branch (nginx_upgrade.inc:1424-1426)
#   - Always uses 'master' branch if NGINX_STICKYVER='master' (nginx_configure.inc:2182)
#   - Removes existing master tarball and re-downloads on each build
#   - Adds sticky directive support for upstream {} blocks in nginx.conf
#   - Creates sticky session cookies for backend server affinity
# NGINX VERSION REQUIREMENTS:
#   - Minimum: Nginx 1.0.0+ (basic nginx-sticky-module support)
#   - Recommended: Nginx 1.18.0+ for stable integration
#   - Modern versions use master branch for nginx 1.25.x+ compatibility
# UPGRADE BEHAVIORS: (from nginx_upgrade.inc)
#   - Line 1424-1426: if NGINX_STICKY='y' removes old nginx-sticky-module-master.tar.gz and re-downloads
#   - Always pulls latest master commit (unpredictable versioning)
#   - May break existing installations if master has breaking changes
# PATCH BEHAVIORS:
#   - No specific patches in nginx_patch.inc (upstream module maintained separately)
# CONFIGURE FLAGS:
#   - --add-module=../nginx-sticky-module-master (or versioned directory)
#   - nginx_configure.inc:2179,2181,2796: Displays NGINX_STICKY status and adds module
# OS COMPATIBILITY:
#   - EL6/7/8/9/10: Full compatibility across all Enterprise Linux versions
#   - No special dependencies beyond standard nginx build requirements
# PERFORMANCE IMPACT:
#   - Minimal: Cookie-based session persistence (~100 bytes cookie)
#   - No backend connection overhead (decisions made at nginx level)
#   - Recommended for: Load-balanced applications requiring session affinity
# SECURITY IMPLICATIONS:
#   - Cookies expose backend server information (server IP/hash in cookie)
#   - Cookie tampering could force requests to specific backend servers
#   - Recommended: Use secure cookie attributes (httponly, secure, samesite)
#   - Alternative: IP hash load balancing (ip_hash directive) for privacy
# CONFLICTS:
#   - No conflicts with other nginx modules
#   - Works with upstream module for HTTP, FastCGI, uWSGI load balancing
# RECOMMENDATION:
#   - Enable for load-balanced web applications requiring session persistence
#   - Recommended for: PHP sessions, Java Tomcat, stateful applications
#   - Disable for: Stateless APIs, single backend server setups
#   - Modern alternative: Use application-level session storage (Redis, Memcached)
# Related: NGINX_STICKYVER, upstream load balancing configuration
# Reference: https://github.com/bymaximus/nginx-sticky-module-ng
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

# OpenResty Lua Nginx Module Integration
# Controls installation of lua-nginx-module (OpenResty's Lua module) for embedding Lua into nginx worker processes.
# AUTOMATIC BEHAVIOR:
#   - When ORESTY_LUANGINX='y': Downloads lua-nginx-module-${ORESTY_LUANGINXVER} from GitHub (nginx_upgrade.inc:367)
#   - Automatically installs LuaJIT 2.1 runtime and lua-resty-core/lua-resty-lrucache libraries
#   - Installs ngx_devel_kit module (required dependency for lua-nginx-module)
#   - Creates Lua module paths: /usr/local/lib/lua/5.1/, /usr/local/share/lua/5.1/
#   - Automatically disabled when incompatible with nginx version (nginx_configure.inc:478,492)
# NGINX VERSION REQUIREMENTS:
#   - Minimum: Nginx 1.11.6+ for modern lua-nginx-module versions
#   - Nginx 1.23.0+ with lua-nginx-module 0.10.21 requires patches (nginx_patch.inc:776)
#   - Nginx 1.25.4+ with stream module requires lua-stream-nginx-module patches (nginx_patch.inc:814)
#   - Nginx 1.27.0-1.28.0 with NGINX_OPENRESTY='y' requires compatibility patches (nginx_patch.inc:5)
# UPGRADE BEHAVIORS: (from nginx_upgrade.inc)
#   - Line 72,149: if ORESTY_LUANGINX='y' displays module status during upgrade
#   - Line 87,164: grep -E 'NGINX_PCRE|NGINX_MODSECURITY|ORESTY_LUANGINX' shows related config
#   - Line 367: Downloads lua-nginx-module-${ORESTY_LUANGINXVER} tarball during upgrade
#   - Line 406: Removes old lua-nginx-module source before extracting new version
#   - Line 515-516: if ORESTY_LUANGINX='y' adds --add-module for lua-nginx-module
#   - Line 778: Displays figlet banner for Lua nginx module during upgrade
#   - Line 994,1123: Echoes ORESTY_LUANGINX=$ORESTY_LUANGINX in debug output
# PATCH BEHAVIORS: (from nginx_patch.inc)
#   - Line 5: Nginx 1.27.0-1.28.0 + ORESTY_LUANGINX='y' + NGINX_OPENRESTY='y' applies compat patches
#   - Line 776: lua-nginx-module 0.10.21 with nginx 1.23.0+ gets socket error fixes
#   - Line 814: Nginx 1.25.4+ with NGX_LUASTREAM='y' gets lua-stream-nginx-module patches
#   - Line 856: lua-nginx-module 0.10.7/0.10.8 with nginx 1.11.11+ gets API compatibility patches
#   - Line 868,877: Version-specific patches for 0.10.7 and 0.10.8 respectively
#   - Line 1339-1341: Downloads and applies lua-nginx-module GitHub commit patches
# CONFIGURE FLAGS:
#   - DYNAMIC_SUPPORT='y': --add-dynamic-module=../lua-nginx-module-${ORESTY_LUANGINXVER}
#   - Static build: --add-module=../lua-nginx-module-${ORESTY_LUANGINXVER}
#   - Requires: --add-module=../ngx_devel_kit-${NGX_DEVEL_VER} (dependency)
#   - Adds: LUALD_OPT='-Wl,-rpath,/usr/local/lib' for LuaJIT linking
#   - Adds: LUAJIT_XCFLAGOPT='LUA_LIB=/usr/local/lib LUA_INC=/usr/local/include/luajit-2.1'
# OS COMPATIBILITY:
#   - EL6: LuaJIT 2.0.x, lua-nginx-module 0.10.x (older versions)
#   - EL7/8/9/10: LuaJIT 2.1.x, lua-nginx-module 0.10.21+ (modern versions)
#   - AlmaLinux/Rocky/Oracle: Full compatibility with latest LuaJIT 2.1.x
#   - Requires gcc/g++ for LuaJIT compilation
# PERFORMANCE IMPACT:
#   - Adds ~3-5MB to nginx binary size (depends on DYNAMIC_SUPPORT)
#   - LuaJIT provides near-native performance (within 5-10% of C)
#   - Lua scripts execute in nginx worker context (no external process overhead)
#   - Memory: ~2-5MB per worker for LuaJIT VM, minimal if Lua not actively used
# SECURITY IMPLICATIONS:
#   - Lua code runs with nginx worker privileges (same as nginx master)
#   - Improperly written Lua scripts can crash nginx workers
#   - lua-resty-core provides sandboxed Lua execution environment
#   - Enables powerful WAF capabilities (ModSecurity alternative via Lua)
# CONFLICTS:
#   - Requires ngx_devel_kit module (automatically installed)
#   - May conflict with some nginx modules if load order incorrect
#   - Check nginx_configure.inc:786,806 for PCRE/ModSecurity compatibility notes
# RECOMMENDATION:
#   - Enable for advanced caching (redis integration, custom cache keys)
#   - Recommended for API gateways, dynamic routing, and WAF functionality
#   - Disable if only serving static content or simple PHP applications
#   - Use lua-resty-core for security-hardened Lua functions
#   - Pair with NGINX_OPENRESTY='y' for full OpenResty component suite
# Related: NGINX_OPENRESTY, ORESTY_LUANGINXVER, NGX_DEVEL_VER, NGX_LUASTREAM
# Reference: https://github.com/openresty/lua-nginx-module
# Git History: Added with OpenResty integration ~2014-2015, active development ongoing
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

# System OpenSSL Library for Nginx
# Use system-installed OpenSSL instead of compiling custom OpenSSL with Nginx.
# AUTOMATIC BEHAVIOR:
#   - Auto-disabled when LIBRESSL_SWITCH='y' (line 474-478)
#   - Auto-disabled when BORINGSSL_SWITCH='y' (line 479-481)
#   - Auto-disabled when NGINX_QUIC_SUPPORT='y' (line 482-484) - QUIC needs custom quicTLS
#   - Auto-disabled when AWS_LC_SWITCH='y' (line 488-492) - AWS-LC is custom library
# OS COMPATIBILITY:
#   - EL6: OpenSSL 1.0.1e (outdated, not recommended)
#   - EL7: OpenSSL 1.0.2k (legacy, not recommended)
#   - EL8: OpenSSL 1.1.1k (acceptable for non-TLS1.3 deployments)
#   - EL9: OpenSSL 3.0.7 (recommended)
#   - EL10: OpenSSL 3.2.x (recommended)
# CONFIGURE FLAGS:
#   - No custom --with-openssl flag
#   - Links against /usr/lib64/libssl.so and /usr/lib64/libcrypto.so
#   - Uses system OpenSSL headers from /usr/include/openssl/
# CONFLICTS:
#   - Disabled by LIBRESSL_SWITCH, BORINGSSL_SWITCH, AWS_LC_SWITCH
#   - Incompatible with NGINX_QUIC_SUPPORT (QUIC requires custom quicTLS fork)
#   - Not recommended for NGINX_KTLS (kernel TLS prefers OpenSSL 3.x custom build)
# USE CASES:
#   - Quick testing environments
#   - OS vendor security update reliance (yum/dnf update openssl)
#   - Centralized OpenSSL patch management
# LIMITATIONS:
#   - No HTTP/3 QUIC support (requires quicTLS fork)
#   - May lack performance patches from custom OpenSSL builds
#   - Limited control over TLS 1.3 configuration
#   - EL8/older: Outdated OpenSSL versions with missing features
# SECURITY CONSIDERATIONS:
#   - OS vendor provides security updates
#   - May lag behind upstream OpenSSL releases
#   - EL7 OpenSSL 1.0.2 lacks TLS 1.3 entirely
# RECOMMENDATION: Disable ('n') for production. Custom OpenSSL compilation provides:
#   - Latest TLS 1.3 features
#   - Performance patches (ECDH optimizations)
#   - HTTP/3 QUIC support (via quicTLS)
#   - Kernel TLS offload (OpenSSL 3.x)
# Related: LIBRESSL_SWITCH, BORINGSSL_SWITCH, AWS_LC_SWITCH, NGINX_QUIC_SUPPORT
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
# ChaCha20-Poly1305 Cipher Prioritization for Mobile Clients
# Moves ChaCha20-Poly1305 cipher suites to front of SSL cipher list for better mobile device performance.
# AUTOMATIC BEHAVIOR:
#   - When NGINX_PRIORITIZECHACHA='y' and OpenSSL 1.1.1+: Applies cipher priority patch (nginx_patch.inc:376)
#   - Modifies SSL_CTX_set_cipher_list() in nginx source to prioritize ChaCha20 ciphers
#   - Automatically reorders cipher suites: ChaCha20-Poly1305 > AES-GCM > AES-CBC
#   - Applies only when OpenSSL 1.1.1+ detected (ChaCha20 support required)
# NGINX VERSION REQUIREMENTS:
#   - Minimum: Nginx 1.15.4+ (nginx_patch.inc:376: DETECT_NGXVER >= 1015004)
#   - OpenSSL 1.1.1+ required for ChaCha20-Poly1305 support
#   - Works with nginx 1.25.x+ and OpenSSL 3.x
# UPGRADE BEHAVIORS: (from nginx_upgrade.inc)
#   - No specific upgrade logic, patch applied during compilation if conditions met
#   - Preserves ChaCha20 prioritization after upgrades
# PATCH BEHAVIORS: (from nginx_patch.inc)
#   - Line 376: if NGINX_PRIORITIZECHACHA='y' and nginx >= 1.15.4 and OpenSSL 1.1.1 applies patch
#   - Patch file: patches/nginx/nginx-ssl-prioritize-chacha.patch
#   - Modifies ngx_ssl_create() function to reorder cipher list
# CONFIGURE FLAGS:
#   - No direct ./configure flags, patch modifies runtime SSL cipher negotiation
#   - Works with existing SSL configuration (no vhost changes needed)
# OS COMPATIBILITY:
#   - EL7: OpenSSL 1.0.2 (no ChaCha20 support, patch has no effect)
#   - EL8/9/10: OpenSSL 1.1.1+ (full ChaCha20 support)
#   - Requires: OpenSSL 1.1.1+ compiled with ChaCha20-Poly1305 support
# PERFORMANCE IMPACT:
#   - Mobile devices without AES-NI: ChaCha20 is 3x faster than AES-GCM
#   - Desktop/server CPUs with AES-NI: AES-GCM is faster (hardware acceleration)
#   - Battery life: ChaCha20 uses 30-50% less CPU on mobile devices
#   - TLS handshake time: No change, only cipher selection affected
# SECURITY IMPLICATIONS:
#   - ChaCha20-Poly1305 is considered equally secure to AES-256-GCM
#   - Recommended by IETF (RFC 8439) and widely deployed
#   - Used by default in Cloudflare, Google services for mobile clients
#   - No security downgrade, only cipher preference reordering
# CONFLICTS:
#   - No conflicts with other nginx modules
#   - Works with all SSL libraries supporting OpenSSL 1.1.1+ API
#   - Compatible with Let's Encrypt, ACME, and all certificate types
# RECOMMENDATION:
#   - Enable for sites with significant mobile traffic (>30% mobile users)
#   - Recommended for: Mobile-first web apps, API servers with mobile clients
#   - Disable for: Internal corporate sites (desktop-only), legacy browser support
#   - Pair with modern TLS 1.3 configuration for best mobile performance
# Related: NGINX_VHOSTSSL, LETSENCRYPT_DETECT, OpenSSL variables
# Reference: https://blog.cloudflare.com/it-takes-two-to-chacha-poly/
# Git History: Added 2018-09-26 (bde188b1)
#NGINX_PRIORITIZECHACHA='n'  

# Cloudflare Optimized Zlib Library
# Replace standard zlib with Cloudflare's optimized fork featuring SSE4.2/PCLMUL acceleration.
# AUTOMATIC BEHAVIOR:
#   - Auto-disabled when NGINX_ZLIBNG='y' (line 710-717) - zlib-ng takes priority
#   - Requires CPU SSE4.2 and PCLMUL instruction support (line 714-734)
#   - CPU detection: grep /proc/cpuinfo for 'sse4_2' and PCLMUL flags
#   - Auto-sets CLOUDFLARE_ZLIBRAUTOMAX for optimal compression level (line 726-733)
# NGINX VERSION COMPATIBILITY:
#   - All Nginx versions compatible
#   - Works with HTTP/2 gzip compression
# CPU REQUIREMENTS:
#   - x86_64 architecture required (line 710)
#   - SSE4.2 instruction set (Intel Nehalem+ / AMD Bulldozer+)
#   - PCLMUL (AES-NI carryless multiply) instruction
#   - Detection: cat /proc/cpuinfo | grep -o 'sse4_2|pclmul'
# CONFIGURE FLAGS:
#   - --with-zlib=../zlib-cloudflare-${CLOUDFLARE_ZLIBVER}
#   - Uses ZLIBCUSTOM_OPT variable
#   - ZLIBCUSTOM_DIR="zlib-cloudflare-${CLOUDFLARE_ZLIBVER}"
# CONFLICTS:
#   - Mutually exclusive with NGINX_ZLIBNG='y' (zlib-ng has priority)
#   - Mutually exclusive with NGINX_ZLIBCUSTOM='n'
# DYNAMIC MODULE SUPPORT:
#   - CLOUDFLARE_ZLIB_DYNAMIC='y' enables dynamic zlib loading
#   - Requires Nginx 1.11.5+ for dynamic zlib support
# PERFORMANCE IMPACT:
#   - Compression speed: 50-300% faster than stock zlib (CPU-dependent)
#   - Decompression speed: 30-100% faster
#   - CPU usage: Similar or lower (hardware acceleration)
#   - Best performance on Intel Xeon E5+ / AMD EPYC processors
# COMPRESSION OPTIMIZATION:
#   - CLOUDFLARE_ZLIBRAUTOMAX='y' adjusts gzip_comp_level automatically
#   - Sets optimal level based on Cloudflare's research
#   - Modifies nginx.conf gzip_comp_level directive
# USE CASES:
#   - High-traffic HTTP/2 sites with gzip compression
#   - Modern Intel/AMD processors with SSE4.2+PCLMUL
#   - Performance-critical gzip workloads
# RECOMMENDATION: Enable on modern CPUs (2010+ Intel, 2011+ AMD) for significant
#   gzip performance gains. Disable if CPU lacks SSE4.2 or prefer zlib-ng.
# Related: NGINX_ZLIBNG, CLOUDFLARE_ZLIB_DYNAMIC, CLOUDFLARE_ZLIBRAUTOMAX
# Reference: https://github.com/cloudflare/zlib
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
#CLOUDFLARE_ZLIBRAUTOMAX='n'

# Cloudflare zlib for PHP
# Compile PHP-FPM with Cloudflare's optimized zlib fork instead of system zlib
# REQUIREMENTS (Auto-checked):
#   - CPU: SSE4.2 support (Intel Nehalem+, AMD Bulldozer+)
#   - CPU: PCLMUL support (AES-NI instruction set)
#   - Auto-disabled if CPU lacks required features
# PERFORMANCE GAINS:
#   - gzip compression: 2-3x faster than system zlib
#   - gzip decompression: 1.5-2x faster than system zlib
#   - Lower CPU usage for PHP gzip operations
# USE CASES:
#   - PHP applications with heavy gzip usage
#   - WordPress sites using gzip compression
#   - APIs serving compressed JSON responses
# CONFLICTS:
#   - PHP_CUSTOMSSL='y': May conflict with custom OpenSSL RPATH
#   - LOWMEM_INSTALL='y': Compilation requires extra memory
# VERSION:
#   - Uses version from CLOUDFLARE_ZLIBVER variable
#   - Cloudflare maintains fork at https://github.com/cloudflare/zlib
# AUTOMATIC DISABLE:
#   - Old CPUs without SSE4.2/PCLMUL
#   - Non-x86_64 architectures
# RECOMMENDATION: Enable on modern CPUs (2010+) for gzip-heavy workloads
# Related: CLOUDFLARE_ZLIB, CLOUDFLARE_ZLIBVER, PHP_CUSTOMSSL
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

# LibreSSL Library Switch for Nginx
# Replace OpenSSL with OpenBSD's LibreSSL fork for enhanced security and code audit.
# AUTOMATIC BEHAVIOR:
#   - Auto-sets OPENSSL_SYSTEM_USE='n' (line 474-478)
#   - Auto-disables ORESTY_LUASTREAM='n' (line 477) - Lua incompatibility
#   - Auto-disables ORESTY_LUANGINX='n' (line 478) - Lua incompatibility
#   - luaworkaroundfix() checks Lua version and reverts to OpenSSL if needed (line 432-441)
# NGINX VERSION REQUIREMENTS:
#   - Nginx 1.13.6+ recommended for LibreSSL 3.x
#   - Nginx 1.11.x+ minimum for LibreSSL 2.x
# OS COMPATIBILITY:
#   - EL6/7/8/9/10: Supported
# CONFIGURE FLAGS:
#   - --with-cc-opt: LIBRESSLINC_OPT (include paths)
#   - --with-ld-opt: LIBRESSL_LIBOPT (library linking)
#   - --with-openssl=/path/to/libressl
#   - Uses LIBRESSLOPT variable in configure line
# CONFLICTS:
#   - Mutually exclusive with AWS_LC_SWITCH='y'
#   - Mutually exclusive with BORINGSSL_SWITCH='y'
#   - Mutually exclusive with OPENSSL_SYSTEM_USE='y'
#   - Incompatible with ORESTY_LUASTREAM and ORESTY_LUANGINX (Lua modules)
#   - Incompatible with NGINX_QUIC_SUPPORT (QUIC requires OpenSSL 1.1.1-quic)
# COMPILATION FIXES:
#   - libresslcompile_fix() modifies auto/lib/openssl/make (line 443-451)
#   - Changes 'install_sw' to 'install' for LibreSSL compatibility
# PERFORMANCE IMPACT:
#   - Compilation: Similar to OpenSSL
#   - Runtime: Comparable to OpenSSL 1.1.1 performance
#   - Memory: Slightly lower memory footprint
# CRYPTOGRAPHIC FEATURES:
#   - Modern TLS 1.3 support (LibreSSL 3.4+)
#   - ChaCha20-Poly1305 ciphers
#   - Removed legacy SSLv2/SSLv3 code entirely
#   - OpenBSD security audit and cleanup
# USE CASES:
#   - Security-focused deployments (OpenBSD-level code quality)
#   - Environments without Lua module requirements
#   - Legacy protocol elimination (no SSLv2/SSLv3 attack surface)
# SECURITY CONSIDERATIONS:
#   - Aggressive legacy protocol removal
#   - OpenBSD security team maintenance
#   - Smaller attack surface than OpenSSL
# RECOMMENDATION: Only enable if you specifically need OpenBSD's crypto stack and
#   do not use Lua modules or QUIC. OpenSSL 3.x provides broader compatibility.
# Related: AWS_LC_SWITCH, BORINGSSL_SWITCH, ORESTY_LUASTREAM, NGINX_QUIC_SUPPORT
# Reference: https://www.libressl.org/
# Git History: First added 2015-06-03 (a823dd20), Last modified 2017-05-10 (fdfc91e4)
#LIBRESSL_SWITCH='n'  

# LibreSSL Version
# LibreSSL version to compile
# Reference: http://www.libressl.org/
# Git History: First added 2015-06-03 (a823dd20), Last modified 2016-02-15 (7240145c)
#LIBRESSL_VERSION='3.9.2'  

# Google BoringSSL Library Switch for Nginx
# Replace OpenSSL with Google's BoringSSL fork for modern TLS 1.3 and QUIC support.
# AUTOMATIC BEHAVIOR:
#   - Auto-sets OPENSSL_SYSTEM_USE='n' (line 479-481)
#   - Auto-disables NGINX_FATLTO_OBJECTS (line 502) - LTO incompatible with BoringSSL
#   - Checks for BoringSSL at multiple points (line 479, 614, 862)
# NGINX VERSION REQUIREMENTS:
#   - Nginx 1.13.10+ minimum for BoringSSL support
#   - QUIC support: Works with nginx-quic builds
# OS COMPATIBILITY:
#   - EL6/7/8/9/10: Supported (CMake required for build)
# CONFIGURE FLAGS:
#   - --with-cc-opt: BORINGSSLINC_OPT (include paths)
#   - --with-ld-opt: BORINGSSL_LIBOPT (library linking)
#   - --with-openssl=/path/to/boringssl
#   - Uses BORINGSSLOPT variable in configure line
# CONFLICTS:
#   - Mutually exclusive with AWS_LC_SWITCH='y'
#   - Mutually exclusive with LIBRESSL_SWITCH='y'
#   - Mutually exclusive with OPENSSL_SYSTEM_USE='y'
#   - Disables NGINX_FATLTO_OBJECTS (LTO incompatibility)
# COMPILATION REQUIREMENTS:
#   - CMake 3.x+ required (BoringSSL build system)
#   - Go compiler required for BoringSSL assembly
#   - Longer compilation time vs OpenSSL
# PERFORMANCE IMPACT:
#   - Compilation: 2-3x longer than OpenSSL (CMake + Go builds)
#   - Runtime: Similar or slightly faster TLS 1.3 handshakes
#   - Binary size: Smaller than OpenSSL
# CRYPTOGRAPHIC FEATURES:
#   - Modern TLS 1.3 ciphersuites
#   - QUIC crypto primitives
#   - Constant-time operations (timing attack resistance)
#   - Aggressive deprecation of legacy crypto
# USE CASES:
#   - Google Cloud Platform deployments (aligned with GCP standards)
#   - Modern TLS 1.3-only environments
#   - QUIC/HTTP3 with Google's crypto stack
# SECURITY CONSIDERATIONS:
#   - No legacy protocol support (only TLS 1.2+)
#   - Google's security-first development model
#   - Frequent breaking API changes (not API-stable like OpenSSL)
# RECOMMENDATION: Only enable if you specifically need Google's crypto stack or
#   Google Cloud Platform alignment. OpenSSL 3.x provides better compatibility.
# Related: AWS_LC_SWITCH, LIBRESSL_SWITCH, NGINX_FATLTO_OBJECTS, OPENSSL_SYSTEM_USE
# Reference: https://boringssl.googlesource.com/boringssl/
# Git History: Added 2016-06-02 (f761a456)
#BORINGSSL_SWITCH='n'  

# BoringSSL Shared Library
# Build BoringSSL as shared library for dynamic Nginx compilation
# Git History: Added 2018-08-19 (8fa9a96c)
#BORINGSSL_SHARED='y'  

# BoringSSL Directory
# Git History: Added 2018-08-16 (617ce44a)
#BORINGSSL_DIR="/opt"

# AWS-LC Cryptographic Library Switch for Nginx
# Replace OpenSSL with AWS's LibCrypto (AWS-LC) for HTTP/3 QUIC and modern crypto optimizations.
# AUTOMATIC BEHAVIOR:
#   - Auto-disables when NGINX_QUIC_SUPPORT='y' (line 493-496) - quicTLS takes priority
#   - Auto-sets OPENSSL_SYSTEM_USE='n' (line 488-492)
#   - Auto-disables ORESTY_LUASTREAM and ORESTY_LUANGINX (Lua incompatible)
#   - Requires EL8/9/10 only (line 486) - not supported on EL6/7
# NGINX VERSION REQUIREMENTS:
#   - Nginx 1.27.0+ minimum for AWS-LC support (line 614 DETECT_NGXVER check)
#   - HTTP/3 QUIC: Nginx 1.25.0+ when AWS_LC_SWITCH='y' (line 614, 1107)
# OS COMPATIBILITY:
#   - EL8/9/10: Fully supported
#   - EL6/7: Not supported (AWS-LC build incompatibilities)
# CONFIGURE FLAGS:
#   - --with-cc-opt: AWS_LC_INC_OPT (include paths)
#   - --with-ld-opt: AWS_LC_LIBOPT (library linking)
#   - --with-openssl=/path/to/aws-lc
# CONFLICTS:
#   - Mutually exclusive with NGINX_QUIC_SUPPORT='y' (quicTLS preferred)
#   - Mutually exclusive with BORINGSSL_SWITCH='y'
#   - Mutually exclusive with LIBRESSL_SWITCH='y'
#   - Incompatible with ORESTY_LUASTREAM and ORESTY_LUANGINX
# PERFORMANCE IMPACT:
#   - Runtime: 5-15% faster crypto operations vs OpenSSL 3.x (AWS optimizations)
#   - Compilation: Similar to OpenSSL builds
#   - FIPS 140-3 compliance available
# CRYPTOGRAPHIC FEATURES:
#   - Post-quantum TLS experiments (ML-KEM, ML-DSA)
#   - Optimized AES-GCM, ChaCha20-Poly1305, P-256 ECDH
#   - AWS Graviton/x86 SIMD optimizations
# USE CASES:
#   - AWS infrastructure deployments (aligned with AWS crypto standards)
#   - Modern crypto performance requirements
#   - HTTP/3 QUIC on Nginx 1.27+ (alternative to quicTLS)
# SECURITY CONSIDERATIONS:
#   - FIPS 140-3 validated builds available
#   - Maintained by AWS cryptography team
#   - Fewer CVEs than OpenSSL historically
# RECOMMENDATION: Enable for AWS deployments on EL8+ with Nginx 1.27+ and HTTP/3 requirements.
#   Disable if using Lua modules or prefer standard quicTLS for QUIC.
# Related: BORINGSSL_SWITCH, LIBRESSL_SWITCH, NGINX_QUIC_SUPPORT, OPENSSL_SYSTEM_USE
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
# Cloudflare Authenticated Origin Pull Certificate Integration
# Enables automatic Cloudflare Origin CA certificate installation for authenticated origin pulls (SSL between CF and origin).
# AUTOMATIC BEHAVIOR:
#   - When VHOST_CFAUTHORIGINPULL='y': Downloads Cloudflare Origin CA certificate during vhost creation (nginx_addvhost.inc:1226)
#   - Installs /usr/local/nginx/conf/ssl/cloudflare/origin-pull-ca.pem
#   - Adds ssl_client_certificate directive to vhost config for Cloudflare authentication
#   - Enables ssl_verify_client on to validate Cloudflare edge certificate
#   - Restricts origin server access to Cloudflare edge network only
# NGINX VERSION REQUIREMENTS:
#   - Minimum: Nginx 1.0.0+ (ssl_client_certificate support)
#   - Recommended: Nginx 1.19.0+ for modern SSL verification features
#   - Requires: Nginx compiled with --with-http_ssl_module
# UPGRADE BEHAVIORS:
#   - No direct upgrade logic in nginx_upgrade.inc
#   - Preserves Cloudflare Origin CA certificate during nginx upgrades
# PATCH BEHAVIORS:
#   - No specific patches in nginx_patch.inc (uses standard SSL verification)
# VHOST BEHAVIORS: (from nginx_addvhost.inc)
#   - Line 1226: if VHOST_CFAUTHORIGINPULL='y' downloads Cloudflare Origin CA certificate
#   - Downloads: https://support.cloudflare.com/hc/en-us/article_attachments/360044928032/origin-pull-ca.pem
#   - Vhost config adds:
#     ssl_client_certificate /usr/local/nginx/conf/ssl/cloudflare/origin-pull-ca.pem;
#     ssl_verify_client on;
#   - Blocks non-Cloudflare connections (clients must present Cloudflare edge cert)
# CONFIGURE FLAGS:
#   - No nginx ./configure flags affected (uses standard SSL verification)
#   - Requires: --with-http_ssl_module for SSL support
# OS COMPATIBILITY:
#   - EL6/7/8/9/10: Full compatibility across all Enterprise Linux versions
#   - Requires: curl/wget for downloading Cloudflare Origin CA certificate
#   - OpenSSL 1.0.1+ required for certificate verification
# PERFORMANCE IMPACT:
#   - SSL verification: <1ms per connection (Cloudflare edge cert validation)
#   - Minimal CPU/memory overhead (certificate cached after first verification)
#   - No additional handshake latency (verification during TLS handshake)
# SECURITY IMPLICATIONS:
#   - Ensures connections to origin server come from Cloudflare edge network only
#   - Prevents direct-to-IP attacks bypassing Cloudflare protection (DDoS, WAF bypass)
#   - Authenticated Origin Pulls: Cloudflare edge presents client certificate to origin
#   - Recommended for: Sites using Cloudflare for DDoS protection, WAF, CDN
#   - Limitations: Requires Cloudflare Pro plan or higher for Authenticated Origin Pulls
# CONFLICTS:
#   - No conflicts with other nginx modules
#   - Compatible with NGINX_VHOSTSSL='y', LETSENCRYPT_DETECT='y'
#   - Works with any SSL certificate on origin (self-signed, Let's Encrypt, commercial)
# RECOMMENDATION:
#   - Enable for production sites using Cloudflare CDN with SSL/TLS
#   - Recommended for: High-security sites preventing direct-to-IP attacks
#   - Disable if: Not using Cloudflare, or using Cloudflare Free plan (no Authenticated Origin Pulls)
#   - Pair with Cloudflare SSL/TLS settings: Full (strict) encryption mode
# Related: NGINX_VHOSTSSL, LETSENCRYPT_DETECT, Cloudflare SSL settings
# Reference: https://developers.cloudflare.com/ssl/origin-configuration/authenticated-origin-pull/
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
# Enable hardened compilation flags for enhanced PHP-FPM security
# SECURITY FLAGS ADDED:
#   - -D_FORTIFY_SOURCE=2: Buffer overflow detection
#   - -fstack-protector-strong: Stack smashing protection
#   - -Wformat -Werror=format-security: Format string vulnerability detection
# LINKER FLAGS (when PHP_SECURED_PIE='y'):
#   - -Wl,-z,relro: Read-only relocations
#   - -Wl,-z,now: Immediate symbol binding
#   - -pie: Position-independent executable
# SECURITY BENEFITS:
#   - Runtime buffer overflow detection
#   - Stack canary protection against ROP attacks
#   - ASLR (Address Space Layout Randomization) when PIE enabled
# PERFORMANCE IMPACT:
#   - Negligible (~1-2% slower)
#   - Worth the security trade-off
# CHECKSEC VALIDATION:
#   - Can be verified with: checksec --file=/usr/local/sbin/php-fpm
#   - Expected: RELRO, Stack Canary, NX, PIE (if PHP_SECURED_PIE='y')
# USE CASES:
#   - Production servers (highly recommended)
#   - Security-conscious environments
#   - Compliance requirements (PCI-DSS, etc.)
# RECOMMENDATION: Keep enabled for all production deployments
# Related: PHP_SECURED_PIE, NGINXCOMPILE_PIE, NGINX_SECURED
# Git History: Added 2023-11-30 (bb4166df)
#PHP_SECURED='y'  

# PHP Position-Independent Executable (PIE)
# Compile PHP-FPM as PIE for ASLR (Address Space Layout Randomization) support
# SECURITY MECHANISM:
#   - PIE: Randomizes base address of PHP-FPM binary
#   - ASLR: Randomizes memory layout at runtime
#   - Makes exploits harder (attackers can't predict memory addresses)
# COMPILATION FLAGS:
#   - CFLAGS: -fPIC (Position-Independent Code)
#   - LDFLAGS: -pie (Position-Independent Executable)
# REQUIRED FOR:
#   - Modern Linux kernel ASLR (requires PIE binaries)
#   - Full ASLR coverage (stack, heap, libraries, executable)
# CHECKSEC VERIFICATION:
#   - checksec --file=/usr/local/sbin/php-fpm
#   - Expected output: PIE enabled
# PERFORMANCE:
#   - Minimal impact (<1% overhead)
#   - Modern CPUs handle PIE efficiently
# COMPATIBILITY:
#   - Requires: PHP_SECURED='y' (provides base security flags)
#   - Works with: All PHP versions 7.0+
# SECURITY BENEFITS:
#   - Defeats return-to-libc attacks
#   - Prevents ROP (Return-Oriented Programming) gadget chains
#   - Industry best practice (enabled by default on modern distros)
# RECOMMENDATION: Keep enabled (part of defense-in-depth strategy)
# Related: PHP_SECURED, NGINXCOMPILE_PIE
# Git History: Added 2023-12-01 (0b24e133)
#PHP_SECURED_PIE='y'  

# PHP gsplit-dwarf Optimization
# Split DWARF debug info into separate .dwo files to reduce disk I/O during compilation
# COMPILATION MECHANISM:
#   - CFLAGS: -gsplit-dwarf
#   - Splits debug symbols from .o files into .dwo files
#   - Reduces object file size by ~60-70%
# BENEFITS:
#   - Faster linking: Linker doesn't process debug symbols
#   - Lower disk I/O: Smaller .o files = faster disk writes
#   - Reduced memory usage during linking
# COMPILATION SPEED IMPROVEMENT:
#   - 8-core system: ~10-15% faster PHP compilation
#   - 16+ core system: ~15-25% faster (more parallel make jobs)
#   - Biggest gains on HDD (vs SSD)
# FILE SIZE COMPARISON:
#   - Without: .o files ~500-800 KB each
#   - With: .o files ~150-250 KB, .dwo files ~400-600 KB
#   - Total size same, but split improves build performance
# DEBUG SYMBOL HANDLING:
#   - Debug symbols preserved in .dwo files
#   - Can be stripped with STRIPPHP='y'
#   - gdb can still use .dwo files for debugging
# COMPATIBILITY:
#   - GCC 4.8+: Full support
#   - Clang: Limited support (prefer GCC)
# CONFLICTS:
#   - PHPDEBUGMODE='y': May interfere with debug workflows
#   - Old GCC <4.8: Not supported
# RECOMMENDATION: Keep enabled for faster PHP compilations
# Related: NGX_GSPLITDWARF, STRIPPHP, PHPDEBUGMODE
# Reference: https://community.centminmod.com/posts/44072/
# Git History: Added 2017-01-31 (616146e2)
#PHP_GSPLITDWARF='y'  

# PHP LTO (Link Time Optimization)
# Enable -flto compiler flag for whole-program optimization during linking
# CURRENT STATUS: Experimental, default disabled
# REQUIREMENTS:
#   - Minimum: 16 CPU cores AND 31GB+ RAM (auto-detection)
#   - Override: PHP_LTO_FORCE='y' bypasses requirements
# AUTOMATIC DISABLE TRIGGERS:
#   - <16 CPU cores AND <31GB RAM: LTO disabled for safety
#   - Low memory systems: Prevents compilation OOM errors
# COMPILATION IMPACT:
#   - Make threads reduced by 30-50% when LTO enabled
#   - Memory usage: +2-4GB during LTO linking phase
#   - Compilation time: +20-40% longer
# PERFORMANCE GAINS:
#   - PHP 5.6: ~3-5% performance improvement
#   - PHP 7.x: Currently unstable, not recommended
#   - PHP 8.x: Marginal gains, high memory cost
# CURRENT COMPATIBILITY:
#   - Working: PHP 5.6 (legacy)
#   - Broken: PHP 7.0-7.4 (known LTO bugs)
#   - Untested: PHP 8.0+
# RECOMMENDATION: Keep disabled unless testing PHP 5.6 with 16+ cores
# Related: PHP_LTO_FORCE, CPUS_LTO, LOWMEM_INSTALL
# Git History: First added 2017-06-26 (765fa429), Last modified 2017-09-09 (e8c33c03)
#PHP_LTO='n'  

# PHP Custom SSL
# Compile PHP-FPM against custom OpenSSL 1.0.2+ or LibreSSL 2.3+ (matches Nginx SSL library)
# AUTO-BEHAVIOR:
#   - EL9/EL10: Auto-enabled to use system OpenSSL 3.x
#   - EL7: Requires PHP_CUSTOMSSL_FORCE='y' to enable custom OpenSSL paths
#   - EL8+: Uses system OpenSSL by default unless LibreSSL/BoringSSL enabled
# CONTROLS:
#   - SSL library linking and RPATH configuration
#   - OpenSSL version compatibility between PHP and Nginx
# CONFLICTS:
#   - PHP_LDMOLD: Cannot use with custom OpenSSL paths (auto-disabled)
# Related: PHP_CUSTOMSSL_FORCE, OPENSSL_SYSTEM_USE, LIBRESSL_SWITCH, BORINGSSL_SWITCH, PHP_LDMOLD
# Git History: Added 2016-06-23 (bb3cb205)
#PHP_CUSTOMSSL='n'  

# PHP Custom SSL Force
# Force enable PHP_CUSTOMSSL on EL7 systems with custom OpenSSL 1.1.1+ compilation
# REQUIRED FOR: EL7 systems using custom OpenSSL instead of system OpenSSL 1.0.2
# AUTO-BEHAVIOR:
#   - EL7: Disabled by default (use system OpenSSL 1.0.2)
#   - EL8+: Not applicable (uses system OpenSSL 3.x or OPENSSL_SYSTEM_USE)
# SIDE EFFECTS:
#   - Modifies PHP configure paths for OpenSSL libraries
#   - Affects libicu dependency detection on EL7+EL9
#   - Requires custom RPATH for OpenSSL library location
# WARNING: Requires additional LDAP/IMAP symbol conflict handling
# Related: PHP_CUSTOMSSL, OPENSSL_VERSION, NEWLIBICU, PHPIMAP
# Git History: Added 2022-09-09 (4bf3ce2c)
#PHP_CUSTOMSSL_FORCE='n'  

# PHP Patch
# Apply PHP patches if they exist
# Git History: Added 2018-12-09 (d00c75c8)
#PHP_PATCH='y'  

# PHP Patch OpenSSL 3.x Compatibility
# Apply critical OpenSSL 3.x compatibility patches for PHP on EL9/EL10 systems
# AFFECTED PHP VERSIONS:
#   - PHP 7.4: Requires patches for OpenSSL 3.0+ (EL9/10)
#   - PHP 8.0: Requires patches for OpenSSL 3.0+ (EL9/10)
#   - PHP 8.1-8.4: Auto-patched for OpenSSL 3.x compatibility
# OPENSSL VERSION DETECTION:
#   - EL9: OpenSSL 3.0.7+ (system default)
#   - EL10: OpenSSL 3.2.2+ (system default)
#   - Patches applied automatically based on detected version
# PATCHES APPLIED (by PHP version):
#   - PHP 8.1-8.4: OpenSSL 3.x API compatibility (OPENSSL_VERSION_PREREQ checks)
#   - PHP 7.4-8.0: OpenSSL 3.x regression fixes (OPENSSL_VERSION_PREREQ backports)
#   - All versions: Deprecated API workarounds (ENGINE_* functions, etc.)
# PATCH SOURCE:
#   - Stored in: /patches/php/openssl3_*.patch
#   - Backported from: PHP 8.1+ OpenSSL 3.x commits
# AUTOMATIC BEHAVIOR:
#   - Detects OpenSSL 3.0, 3.1, 3.2, 3.3, 3.4 at compile time
#   - Applies version-specific patches based on PHPVER_ID
#   - Skips patches on OpenSSL 1.1.1 systems (EL7/8 custom builds)
# CRITICAL FIXES INCLUDED:
#   - OpenSSL 3.x ENGINE API deprecation handling
#   - SSL_CTX_set_options() flag compatibility
#   - Cipher suite string parsing for OpenSSL 3.x
#   - Certificate verification chain fixes
# RECOMMENDATION: Keep enabled on EL9/10 for all PHP versions
# Related: OPENSSL_VERSION, OPENSSL_SYSTEM_USE, PHP_CUSTOMSSL, PHP_CUSTOMSSL_FORCE
# Git History: Added 2023-10-26 (be96b4fb)
#PHP_PATCH_OPENSSL_THREE='y'  

# PHP MySQL ND Patch Fix
# Backported mysqlnd patch for MariaDB compatibility (PHP <=7.2)
# Reference: https://community.centminmod.com/posts/86953/
# Git History: Added 2020-11-06 (1cb53780)
#PHP_MYSQLND_PATCH_FIX='y'  

# Strip PHP Binary
# Remove debug symbols from PHP-FPM binary to reduce file size
# FILE SIZE IMPACT:
#   - Unstripped: ~25-35 MB (includes debug symbols)
#   - Stripped: ~8-12 MB (~70% size reduction)
# SYMBOLS REMOVED:
#   - Function names for debugging
#   - Line number mappings
#   - Variable names
#   - Source code references
# WHEN TO ENABLE (STRIPPHP='y'):
#   - Production servers (recommended)
#   - Disk space constrained systems
#   - Standard deployments
# WHEN TO DISABLE (STRIPPHP='n'):
#   - Development environments
#   - Debugging with gdb/lldb
#   - Profiling with valgrind/perf
#   - Core dump analysis needed
# CONFLICTS:
#   - PHPDEBUGMODE='y': Auto-disables stripping (debug needs symbols)
# STRIPPING TOOL: Uses 'strip' command on compiled binary
# RECOMMENDATION: Keep enabled for production, disable for development
# Related: PHPDEBUGMODE, STRIPNGINX
# Git History: First added 2015-05-16 (20a9ee07), Last modified 2016-02-15 (7240145c)
#STRIPPHP='y'  

# PHP Make Test
# Set to 'y' to run 'make test' after PHP compilation (diagnostic purposes)
# Git History: First added 2014-06-06 (1f2e1c10), Last modified 2016-02-15 (7240145c)
#PHPMAKETEST=n  # set to y to enable make test after PHP make for diagnostic purposes

# PHP Debug Mode
# Enable PHP debug compilation with symbols, assertions, and diagnostic features
# COMPILATION FLAGS ADDED:
#   - --enable-debug: Enables debug symbols and assertions
#   - Disables compiler optimizations (-O0 instead of -O2/-O3)
#   - Adds memory leak detection hooks
#   - Enables internal consistency checks
# AUTOMATIC SIDE EFFECTS:
#   - Zend OpCache: Forced OFF (incompatible with debug mode)
#   - ZOPCACHE_OVERRIDE: Ignored when PHPDEBUGMODE='y'
#   - Performance: ~30-50% slower due to debug overhead
#   - Binary Size: ~2-3x larger due to debug symbols
# USE CASES:
#   - PHP extension development
#   - Debugging segfaults with gdb
#   - PHP core development
#   - Memory leak investigations
# WARNINGS:
#   - DO NOT use in production (severe performance penalty)
#   - Breaks OpCache acceleration
#   - May expose internal PHP state in errors
# CORE DUMP MANAGEMENT:
#   - PHP_CLEAN_COREDUMPS='y': Auto-removes core dumps when debug disabled
#   - Core dumps stored in PHP-FPM working directory
# RECOMMENDATION: Only enable for development/debugging, never production
# Related: ZOPCACHE_OVERRIDE, ZOPCACHEDFT, PHP_CLEAN_COREDUMPS, STRIPPHP
# Git History: First added 2015-03-08 (cf6145d5), Last modified 2025-05-06 (406ff233)
#PHPDEBUGMODE='n'  

# PHP Clean Core Dumps
# Automatically delete PHP-FPM core dump files from /tmp/ during debug mode cleanup
# AUTOMATIC BEHAVIOR:
#   - Executes only when disabling debug mode (phpdebug_mode_disable function)
#   - Searches /tmp/ directory for core-* files (non-recursive, maxdepth 1)
#   - Deletes all matching core dump files when enabled
#   - Skipped when PHPDEBUGMODE='y' (debug mode active)
# FILE PATTERN:
#   - Target files: /tmp/core-* (PHP-FPM crash dumps)
#   - Command: find /tmp/ -maxdepth 1 -name 'core-*' -delete
# TIMING:
#   - Cleanup occurs when debug mode disabled (menu option or config change)
#   - Does NOT run during normal PHP-FPM operation
# PHP VERSION COMPATIBILITY:
#   - All PHP versions (core dump format universal)
# SECURITY CONSIDERATIONS:
#   - Core dumps may contain sensitive data (passwords, session data)
#   - Recommended for production to avoid disk space issues
#   - Should be disabled for debugging crash investigations
# USE CASES:
#   - Production servers: Enable to auto-cleanup crash dumps
#   - Development servers: Disable to preserve crash evidence
#   - Disk space management: Enable to prevent /tmp/ accumulation
# WARNING:
#   - Deletes debugging evidence needed for crash analysis
#   - Cannot recover deleted core dumps
#   - Disable if investigating PHP-FPM crashes or segfaults
# RECOMMENDATION: Enable for production (default 'y'), disable for debugging crashes
# Related: PHPDEBUGMODE, ZOPCACHE_OVERRIDE, STRIPPHP
# Git History: Added 2025-05-06 (406ff233)
#PHP_CLEAN_COREDUMPS='y'  

# PHP libgd External
# Use external libgd 2.3.3 instead of PHP bundled gd library
# AUTOMATIC BEHAVIOR BY OS:
#   - EL7-9: Uses external libgd 2.3.3 compilation (default: enabled)
#   - EL10: Auto-disabled, uses system libgd 2.3 from repos
# EXTERNAL LIBGD VERSION:
#   - Controlled by: LIBGD_EXTERNAL_VER='2.3.3'
#   - Source: https://github.com/libgd/libgd/releases
# FEATURES IN EXTERNAL LIBGD:
#   - Modern image format support (WebP, AVIF via PHP_GD_AVIF)
#   - Better JPEG/PNG handling
#   - Improved color management
#   - Thread-safe operations
# COMPILATION REQUIREMENTS:
#   - libpng-devel, libjpeg-turbo-devel, freetype-devel
#   - Optional: libwebp-devel (WebP support)
#   - Optional: libavif-devel (AVIF support, EL8+)
# PHP GD CONFIGURE FLAGS:
#   - --with-external-gd: Uses system/compiled libgd
#   - --enable-gd: Enables GD extension
#   - --with-webp: WebP support (if libwebp available)
# AVIF SUPPORT:
#   - Enabled via PHP_GD_AVIF='y' on EL8+
#   - Requires libavif compilation (not in base repos)
# RECOMMENDATION:
#   - EL7-9: Keep enabled for modern image format support
#   - EL10: Auto-disabled (system libgd sufficient)
# Related: PHP_GD_AVIF, LIBGD_EXTERNAL_VER, IMAGEMAGICK_SEVEN
# Git History: First added 2021-12-04 (6305b6ef), Last modified 2025-10-16 (5923fb87)
#PHP_LIBGD_EXTERNAL='y'  

# PHP GD AVIF Support
# Enable AVIF (AV1 Image File Format) support in external libgd for modern image handling
# REQUIREMENTS:
#   - PHP Version: 8.1+ (AVIF support in PHP 8.1+)
#   - OS: EL8, EL9, or EL10 (libavif compilation supported)
#   - PHP_LIBGD_EXTERNAL='y' (requires external libgd)
# AUTOMATIC DEPENDENCIES COMPILED:
#   - libavif: AVIF codec library
#   - libaom: AV1 codec (for AVIF encoding/decoding)
#   - libdav1d: Fast AV1 decoder
#   - x265: HEIF encoding support (optional, via IMAGEMAGICK_HEIF)
# AVIF BENEFITS:
#   - File Size: 30-50% smaller than WebP/JPEG at same quality
#   - Quality: Better compression efficiency than WebP
#   - Use Cases: Modern web images, high-quality thumbnails
# PHP GD FUNCTIONS ENABLED:
#   - imagecreatefromavif(): Load AVIF images
#   - imageavif(): Save images as AVIF
#   - imagepalettetotruecolor(): AVIF requires truecolor
# COMPILATION PROCESS:
#   - 1. Compile libavif with libaom/libdav1d
#   - 2. Compile external libgd with --with-avif
#   - 3. Configure PHP GD extension with AVIF support
# BROWSER SUPPORT (2025):
#   - Chrome 85+, Firefox 93+, Safari 16+, Edge 121+
#   - 95%+ browser support globally
# PERFORMANCE:
#   - Encoding: Slower than JPEG/WebP (CPU-intensive)
#   - Decoding: Fast with libdav1d
# RECOMMENDATION: Enable for modern web applications (PHP 8.1+, EL8+)
# Related: PHP_LIBGD_EXTERNAL, LIBGD_EXTERNAL_VER, IMAGEMAGICK_HEIF, PHPIMAGICK
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
# Use mold (high-speed linker) instead of ld.gold/ld.bfd for PHP-FPM linking
# AUTOMATIC BEHAVIOR:
#   - EL10: Auto-enabled if /usr/bin/mold detected
#   - EL9: Manual enable, installs mold if not present
#   - EL8: Manual enable, installs mold if not present
#   - EL7: Not supported (use NGX_LDMOLD for Nginx only)
# LINKER SPEED COMPARISON:
#   - ld.bfd: ~15-20 seconds (default GNU linker)
#   - ld.gold: ~8-12 seconds (faster GNU linker)
#   - mold: ~2-4 seconds (fastest, 5-10x faster than ld.gold)
# BENEFITS:
#   - Faster PHP compilation (20-40% overall time reduction)
#   - Lower memory usage during linking phase
#   - Better for frequent PHP recompiles
# CONFLICTS:
#   - Cannot use with PHP_CUSTOMSSL (custom OpenSSL paths)
#   - Auto-disabled when PHP_CUSTOMSSL='y'
# INSTALLATION:
#   - EL10: mold installed from AppStream repository
#   - EL9: mold-1.11.0 RPM from Centmin Mod mirror
#   - EL8: mold-1.11.0 RPM from Centmin Mod mirror
# DEFAULT STATE:
#   - EL10: 'n' (auto-detected and enabled internally)
#   - EL8/9: 'n' (manual enable required)
# Related: NGX_LDMOLD, MOLD_VERSION, PHP_CUSTOMSSL
# Git History: Added 2025-10-18 (2a545c44)
#PHP_LDMOLD='n'

# PHP Custom RPMs
# Use custom RPM packages for libsodium/libargon2 on EL10
# Auto-enabled on EL10, ~20-45 seconds faster than source compilation
# Falls back to source if RPM unavailable
# RPM mirror: https://parts.centminmod.com/centminmodparts/rpms/
# Git History: Added 2025-10-18 (42aa40f0)
#PHP_USE_CUSTOM_RPMS='n'

# PHP Overwrite Config Prompt
# Controls interactive prompt during PHP upgrades asking to overwrite php-fpm.conf
# UPGRADE BEHAVIOR:
#   - PHP_OVERWRITECONF='y': Shows interactive prompt to overwrite php-fpm.conf (default)
#   - PHP_OVERWRITECONF='n': Skips prompt, preserves existing php-fpm.conf configuration
# USE CASES:
#   - Automated environments: Set to 'n' to prevent interactive prompts
#   - Manual upgrades: Keep 'y' for safety confirmation
#   - CI/CD pipelines: Set to 'n' for non-interactive operation
# RELATED: PHP_UPDATEMAINTENANCE (enables maintenance mode during upgrades)
# Git History: Added 2014-04-24 (c4b6d98a)
#PHP_OVERWRITECONF='y'

# PHP Update Maintenance Mode
# Controls whether maintenance mode is enabled during PHP-FPM upgrades
# AUTOMATIC BEHAVIOR:
#   - When PHP_UPDATEMAINTENANCE='y': Creates/enables maintenance mode before PHP upgrade
#   - Displays maintenance.html page from /usr/local/nginx/html/ during upgrade
#   - Automatically disables maintenance mode after successful upgrade completion
# UPGRADE BEHAVIORS:
#   - Uses HTTP 503 Service Unavailable response during maintenance
#   - Maintenance page: /usr/local/nginx/html/maintenance.html (customizable)
#   - PHP-FPM reload used for zero-downtime upgrades
# RELATED VARIABLES:
#   - NGINX_UPDATEMAINTENANCE: Maintenance mode for nginx upgrades
#   - MARIADB_UPDATEMAINTENANCE: Maintenance mode for MariaDB upgrades
# Reference: https://community.centminmod.com/posts/26485/
# Git History: Added ~2015-2016 with PHP upgrade workflow improvements
#PHP_UPDATEMAINTENANCE='n'

################################################################################
# 23. PHP PGO & PERFORMANCE SETTINGS
################################################################################

# PHP Profile Guided Optimization (PGO)
# Enable PGO for 5-15% PHP performance improvement via training-based optimization
# REQUIREMENTS:
#   - Minimum: 2+ CPU cores (override with PHP_PGO_ALWAYS='y')
#   - Memory: 4GB+ recommended for PGO compilation
#   - Time: ~2-3x longer compilation (includes training runs)
# AUTO-BEHAVIOR:
#   - PHP 7.0-8.1: Falls back to GCC 12 (workaround GCC 13+ PGO bug)
#   - PHP 8.2+: Uses GCC 13/14 with PGO fixes
#   - Version Label: Appends "-pgo" to PHP version string (controlled by PHP_PGO_APPEND_LABEL)
# GCC VERSION SELECTION (Auto-selected):
#   - EL8 PHP <=8.1: gcc-toolset-12 (PHP_PGO_FALLBACK_GCC='y')
#   - EL9 PHP <=8.1: gcc-toolset-12 (PHP_PGO_FALLBACK_GCC='y')
#   - EL10 PHP <=8.1: gcc-toolset-12 (PHP_PGO_FALLBACK_GCC='y')
#   - Ultra-conservative: gcc-toolset-11 (PHP_PGO_FALLBACK_GCC11='y')
# TRAINING PROCESS:
#   - Training Runs: Controlled by PHP_PGO_TRAINRUNS (default: 10 iterations)
#   - Profile Data: Stored in .gcda files during training
#   - Recompilation: Optimizes binary using collected profile data
# Reference: https://software.intel.com/en-us/blogs/2015/10/09/pgo-let-it-go-php
# Related: PHP_PGO_FALLBACK_GCC, PHP_PGO_FALLBACK_GCC11, PHP_PGO_TRAINRUNS, PHP_PGO_APPEND_LABEL
# Git History: First added 2016-10-01 (8add4514), Last modified 2024-09-29 (43264a84)
#PHP_PGO='n'  

# PHP PGO Fallback to GCC 12
# Use gcc-toolset-12 instead of GCC 13+ for PHP PGO builds (workaround GCC 13+ PGO bug)
# AUTO-BEHAVIOR:
#   - EL8 >=8.7: Falls back to gcc-toolset-12 for PHP 7.0-8.4 PGO builds
#   - EL9 >=9.1: Falls back to gcc-toolset-12 for PHP 7.4-8.4 PGO builds
#   - EL10: Falls back to gcc-toolset-12 for PHP 7.4-8.4 PGO builds
# KNOWN ISSUES WITH GCC 13+:
#   - PGO training corruption on PHP 7.4-8.1
#   - Profile data misalignment causing segfaults
#   - GCC 12 provides stable PGO for all PHP versions
# OVERRIDES:
#   - PHP_PGO_FALLBACK_GCC11='y': Ultra-conservative fallback to gcc-toolset-11 (PHP <=8.1)
#   - Takes precedence over PHP_PGO_FALLBACK_GCC if both enabled
# DEFAULT STATE: 'y' (enabled) for maximum PGO reliability
# Related: PHP_PGO, PHP_PGO_FALLBACK_GCC11, DEVTOOLSETTWELVE
# Git History: Added 2024-09-29 (43264a84)
#PHP_PGO_FALLBACK_GCC='y'  

# PHP PGO Fallback to GCC 11
# Ultra-conservative gcc-toolset-11 fallback for PHP <=8.1 PGO builds
# PRECEDENCE: Overrides PHP_PGO_FALLBACK_GCC if both enabled
# USE CASES:
#   - Maximum PGO stability for PHP 7.4-8.1
#   - Systems with GCC 12 compatibility issues
#   - Conservative production environments
# TRIGGERS:
#   - EL8: gcc-toolset-11 for PHP 7.0-8.1 PGO
#   - EL9: gcc-toolset-11 for PHP 7.4-8.1 PGO
#   - EL10: gcc-toolset-11 for PHP 7.4-8.1 PGO
# RECOMMENDATION: Use PHP_PGO_FALLBACK_GCC='y' (GCC 12) unless GCC 12 issues exist
# Related: PHP_PGO, PHP_PGO_FALLBACK_GCC, DEVTOOLSETELEVEN
# Git History: Not yet documented in custom_config.inc (exists in php_configure.inc)
#PHP_PGO_FALLBACK_GCC11='n'

# PHP PGO Always Enable
# Override PGO 2+ CPU core requirement to enable PGO on single-core systems
# DEFAULT BEHAVIOR:
#   - PHP_PGO='y' requires 2+ CPU cores for safety
#   - Single-core systems skip PGO automatically
# WHEN ENABLED (PHP_PGO_ALWAYS='y'):
#   - Forces PGO compilation on 1 CPU core systems
#   - WARNING: Compilation time can be 3-5x longer on single-core
#   - May cause memory pressure on <2GB RAM systems
# USE CASES:
#   - VPS with single vCPU but sufficient RAM (4GB+)
#   - Development environments testing PGO builds
#   - Production systems willing to wait for PGO compilation
# RECOMMENDATION: Keep disabled unless single-core system has ample resources
# Related: PHP_PGO, PHP_PGO_TRAINRUNS, LOWMEM_INSTALL
# Git History: Added 2019-04-04 (b67d33ad)
#PHP_PGO_ALWAYS='n'  

# PHP PGO Training Runs
# Number of training iterations during PGO profile collection phase
# TRAINING PROCESS:
#   - Each run executes PHP test suite to collect execution profiles
#   - Profile data merged across all runs for optimization
#   - More runs = better profile coverage, longer compilation time
# VALUE RECOMMENDATIONS:
#   - Default: 10 runs (balanced optimization, ~2-3x compile time)
#   - Fast: 5 runs (~1.5-2x compile time, adequate optimization)
#   - Optimal: 15-20 runs (~3-4x compile time, maximum optimization)
#   - Production: 10 runs recommended for reliability
# TIME IMPACT (example 8-core system):
#   - 5 runs: ~30-45 minutes total PGO compilation
#   - 10 runs: ~45-60 minutes total PGO compilation
#   - 20 runs: ~60-90 minutes total PGO compilation
# APPLIES TO: PHP 7.0+ PGO builds
# Related: PHP_PGO, PHP_PGO_ALWAYS, LOWMEM_INSTALL
# Git History: Added 2018-12-02 (a65b2806)
#PHP_PGO_TRAINRUNS='10'  

# PHP PGO Append Label
# Append "-pgo" label to PHP binary version string for easy identification
# VERSION STRING EXAMPLES:
#   - With label: PHP 8.3.21-pgo (cli) (built: Oct 25 2025)
#   - Without label: PHP 8.3.21 (cli) (built: Oct 25 2025)
# BENEFITS:
#   - Clear identification of PGO-optimized binaries
#   - Useful when managing multiple PHP builds
#   - Helps track optimization status in logs/monitoring
# CONDITIONAL BEHAVIOR:
#   - Only appends when PHP_PGO='y'
#   - Applied during PHP configure phase
#   - Visible in: php -v, phpinfo(), logs
# USE CASES:
#   - Production environments tracking optimization status
#   - A/B testing PGO vs non-PGO builds
#   - Documentation/compliance tracking
# Related: PHP_PGO, PHP_PGO_TRAINRUNS
# Git History: Added 2021-11-26 (d56040d1)
#PHP_PGO_APPEND_LABEL='y'  

# PHP PGO CentOS 6 (LEGACY - No longer used)
# Use latest devtoolset for CentOS 6 PGO builds (required GCC >4.4.7)
# STATUS: Deprecated - EL6 support ended in 140.00beta01
# HISTORICAL CONTEXT:
#   - CentOS 6 shipped with GCC 4.4.7 (too old for PGO)
#   - Required devtoolset-6+ for PGO support
# CURRENT BRANCHES:
#   - 141.00beta01: EL7-10 support only
#   - This variable has no effect
# Related: PHP_PGO (deprecated on EL6)
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
# Increase PHP's default PCRE JIT stack size from 192 KB to 512 KB (or custom value)
# DEFAULT PHP PCRE JIT STACK:
#   - PHP <7.4: 64 KB (very limited, frequent stack overflows)
#   - PHP 7.4-8.0: 192 KB (improved but still limited)
#   - PHP 8.1+: 256 KB (better but may still overflow on complex patterns)
# WHEN ENABLED:
#   - Patches ext/pcre/php_pcre.c to raise stack size
#   - Uses value from PHP_PCREJIT_STACKSIZE (default: 512 KB)
# SUPPORTED STACK SIZES:
#   - 64 KB: Original PHP <7.4 default (not recommended)
#   - 192 KB: PHP 7.4-8.0 default
#   - 256 KB: PHP 8.1+ default
#   - 512 KB: Recommended for complex regex patterns (custom_config.inc default)
# SYMPTOMS OF INSUFFICIENT STACK:
#   - "preg_match(): JIT stack limit exceeded" errors
#   - Complex regex patterns failing unexpectedly
#   - WordPress/Drupal regex operations breaking
# USE CASES:
#   - Complex regex patterns in WordPress/Drupal
#   - Large text processing with preg_match_all()
#   - Nested regex patterns with many groups
# MEMORY IMPACT:
#   - Per-thread memory: +320 KB (from 192 KB to 512 KB)
#   - Total impact: Negligible on modern systems
# PATCH APPLIED:
#   - Modifies: ext/pcre/php_pcre.c
#   - Changes: PCRE2_JIT_STACK_START_SIZE and PCRE2_JIT_STACK_MAX_SIZE
# RECOMMENDATION: Enable if seeing PCRE JIT stack errors in PHP logs
# Related: PHP_PCREJIT_STACKSIZE
# Reference: https://github.com/php/php-src/pull/2910
# Git History: Added 2019-10-27 (25e4b1ab)
#PHP_PCREJIT_STACKSIZE_ADJUST='n'  

# PHP PCRE JIT Stack Size
# Raise PCRE JIT stack size to prevent "JIT stack limit reached" errors on complex regex patterns
# AUTOMATIC BEHAVIOR:
#   - Only applies when PHP_PCREJIT_STACKSIZE_ADJUST='y' is enabled
#   - Patches ext/pcre/php_pcre.c during PHP compilation (menu option 5)
#   - Only modifies if current value is 64, 192, or 256 KB (PHP defaults)
#   - Skips patching if already set to non-default value
# PCRE JIT STACK FORMULA:
#   - PCRE2_JIT_STACK_MAX_SIZE = (PHP_PCREJIT_STACKSIZE * 1024)
#   - Example: PHP_PCREJIT_STACKSIZE='512' → 512 KB max stack size
# PHP DEFAULT STACK SIZES:
#   - PHP 7.2: 64 KB (insufficient for modern regex)
#   - PHP 7.3: 192 KB (better but still limited)
#   - PHP 7.4+: 256 KB (improved but may overflow)
# RECOMMENDED VALUES:
#   - Light usage: 256 KB (PHP 7.4+ default, adequate for most patterns)
#   - Moderate usage: 512 KB (recommended, handles complex patterns)
#   - Heavy usage: 1024 KB (complex regex with deep recursion)
#   - Maximum: 2048 KB (extreme cases, memory cost ~2MB per process)
# MEMORY IMPACT:
#   - Each PHP-FPM process allocates this stack size
#   - 512 KB: ~512 KB per process (minimal impact)
#   - 1024 KB: ~1 MB per process (moderate impact)
#   - Formula: (PHP_PCREJIT_STACKSIZE * number_of_fpm_processes)
# PHP VERSION COMPATIBILITY:
#   - PHP 7.2+: Uses PCRE2 with JIT stack (compatible)
#   - PHP 5.x-7.1: Uses PCRE1 (different stack mechanism)
# REQUIREMENTS:
#   - Requires PHP_PCREJIT_STACKSIZE_ADJUST='y' to activate
#   - Applied during centmin.sh menu option 5 (PHP upgrade/reinstall)
# USE CASES:
#   - WordPress with complex regex in plugins/themes
#   - E-commerce platforms with URL rewrite patterns
#   - Applications using preg_match/preg_replace extensively
#   - Error "JIT stack limit reached" in PHP error logs
# RECOMMENDATION: Set to 512 KB (default), increase to 1024 KB if seeing JIT stack errors
# Related: PHP_PCREJIT_STACKSIZE_ADJUST
# Reference: https://github.com/php/php-src/pull/2910
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
# Enable IMAP (Internet Message Access Protocol) email extension for PHP
# FEATURES PROVIDED:
#   - imap_*() functions for email operations
#   - POP3, IMAP4, NNTP protocol support
#   - Mailbox management and message parsing
# DEPENDENCIES:
#   - uw-imap-devel: UW IMAP C-Client library
#   - Auto-installed from REMI repository
# SPECIAL HANDLING ON EL7 WITH CUSTOM OPENSSL:
#   - PHP_CUSTOMSSL_FORCE='y' + PHPIMAP='y': Requires LDAP symbol conflict fixes
#   - uw-imap links against system OpenSSL 1.0.2
#   - PHP links against custom OpenSSL 1.1.1+
#   - Symbol collision resolution applied automatically
# COMPILATION FLAGS:
#   - --with-imap: Enables IMAP extension
#   - --with-imap-ssl: SSL support for IMAP connections
# USE CASES:
#   - Webmail applications (Roundcube, SquirrelMail)
#   - Email automation scripts
#   - Mail server management tools
# SECURITY NOTES:
#   - uw-imap is legacy software (last update 2007)
#   - Consider modern alternatives (php-mailparse, PECL imap)
# EL9/10 BEHAVIOR:
#   - Uses system OpenSSL 3.x with uw-imap
#   - No symbol conflicts (OpenSSL 3.x has clean symbol namespace)
# RECOMMENDATION: Enable only if IMAP functionality required
# Related: PHP_CUSTOMSSL_FORCE, OPENSSL_VERSION, PHPMAILPARSE
# Git History: Added 2017-07-30 (3d3172e7)
#PHPIMAP='y'  

# PHP Fileinfo Extension
# Enable fileinfo extension for MIME type detection (finfo_*() functions)
# FEATURES PROVIDED:
#   - finfo_open(): Create fileinfo resource
#   - finfo_file(): Detect file MIME type
#   - finfo_buffer(): Detect buffer MIME type
#   - mime_content_type(): Convenience function
# MAGIC DATABASE:
#   - Uses libmagic (file command library)
#   - Magic file: /usr/share/misc/magic.mgc
# COMPILATION MODES:
#   - PHPFINFO='y' + PHPFINFO_STANDALONE='n': Built-in (default, recommended)
#   - PHPFINFO='y' + PHPFINFO_STANDALONE='y': Loadable module (.so)
# BUILT-IN VS STANDALONE:
#   - Built-in: Always available, faster loading, uses less memory
#   - Standalone: Can be disabled via php.ini, uses more memory
# USE CASES:
#   - File upload validation (check real MIME type)
#   - Security: Prevent file extension spoofing
#   - WordPress media library (image/video type detection)
# RECOMMENDATION: Keep disabled unless file type detection needed
# Related: PHPFINFO_STANDALONE
# Git History: First added 2014-04-24 (d962e8b9), Last modified 2016-02-15 (7240145c)
#PHPFINFO='n'  

# PHP Fileinfo Standalone Module
# Compile fileinfo as loadable .so module instead of built-in extension
# WHEN TO USE STANDALONE:
#   - Need to disable fileinfo via php.ini dynamically
#   - Memory-constrained systems (disable when not needed)
#   - Testing fileinfo behavior independently
# LOADING:
#   - Requires: extension=fileinfo.so in php.ini
#   - Location: /usr/local/lib/php/extensions/
# MEMORY USAGE:
#   - Built-in: ~2-3 MB (always loaded)
#   - Standalone: ~2-3 MB (only when loaded)
#   - Difference: Negligible on modern systems
# RECOMMENDATION: Keep disabled (use built-in mode)
# Related: PHPFINFO
# Git History: Added 2019-02-28 (fadc092f)
#PHPFINFO_STANDALONE='n'  

# PHP Process Control Extension (pcntl) - CLI ONLY
# POSIX process control functions for Unix/Linux process management
# CONFIGURE FLAGS:
#   - --enable-pcntl
# FEATURES PROVIDED:
#   - pcntl_fork(): Create child processes (fork system call)
#   - pcntl_signal(): Signal handling (SIGTERM, SIGHUP, SIGUSR1, etc.)
#   - pcntl_wait()/pcntl_waitpid(): Wait for child process termination
#   - pcntl_exec(): Execute programs (exec system call)
#   - pcntl_alarm(): Set alarm signals
#   - Process priority control: pcntl_setpriority(), pcntl_getpriority()
# PHP VERSION COMPATIBILITY:
#   - PHP 5.x: Fully compatible
#   - PHP 7.x: Fully compatible
#   - PHP 8.x: Fully compatible
# OS COMPATIBILITY:
#   - Unix/Linux only: POSIX process control (EL6-10)
#   - Windows: NOT available (POSIX functions unavailable)
# CRITICAL SECURITY WARNING:
#   - DANGEROUS in PHP-FPM/web contexts (remote code execution risk)
#   - MUST be disabled in php-fpm.ini: disable_functions=pcntl_*
#   - Safe ONLY in CLI SAPI (command-line scripts)
#   - Web exposure = critical security vulnerability
# SAFE USE CASES (CLI ONLY):
#   - Laravel Horizon: Queue worker process management
#   - Symfony Console: Long-running command signal handling
#   - Job queues: Worker daemons with graceful shutdown
#   - Cron jobs: Multi-process batch processing
#   - CLI daemons: Background service signal handling
# PRODUCTION SECURITY BEST PRACTICE:
#   - php.ini (CLI): Allow pcntl functions
#   - php-fpm.ini (web): disable_functions=pcntl_alarm,pcntl_fork,pcntl_waitpid,pcntl_wait,pcntl_wifexited,pcntl_wifstopped,pcntl_wifsignaled,pcntl_wexitstatus,pcntl_wtermsig,pcntl_wstopsig,pcntl_signal,pcntl_signal_dispatch,pcntl_get_last_error,pcntl_strerror,pcntl_sigprocmask,pcntl_sigwaitinfo,pcntl_sigtimedwait,pcntl_exec,pcntl_getpriority,pcntl_setpriority
# USE CASES:
#   - Laravel queue workers with signal handling
#   - Multi-process CLI scripts with fork()
#   - Graceful shutdown on SIGTERM/SIGINT
#   - Background job processing daemons
# WARNING:
#   - Compile extension: Enable (for CLI availability)
#   - PHP-FPM: DISABLE via disable_functions (security mandatory)
# RECOMMENDATION: Enable for CLI workers, DISABLE in PHP-FPM via disable_functions
# Related: posix extension, proc_open, exec, shell_exec, Laravel Horizon
# Git History: First added 2014-04-24 (c4b6d98a), Last modified 2016-02-15 (7240145c)
#PHPPCNTL='y'  

# PHP Internationalization Extension (intl)
# ICU (International Components for Unicode) wrapper for Unicode and i18n support
# CONFIGURE FLAGS:
#   - --enable-intl
# DEPENDENCIES:
#   - libicu-devel: ICU library development headers
#   - ICU library: Unicode character set handling
# PHP VERSION COMPATIBILITY:
#   - PHP 5.3+: Fully compatible
#   - PHP 7.x: Fully compatible
#   - PHP 8.x: Fully compatible
# FEATURES PROVIDED:
#   - Locale: Language/region-specific formatting
#   - Collator: Unicode-aware string comparison and sorting
#   - NumberFormatter: International number formatting (currency, percentages)
#   - IntlDateFormatter: Date/time formatting per locale
#   - MessageFormatter: ICU message formatting with placeholders
#   - Grapheme: Unicode grapheme cluster handling
#   - IDN: Internationalized Domain Names (Punycode)
#   - Normalizer: Unicode normalization (NFC, NFD, NFKC, NFKD)
#   - IntlChar: Unicode character properties
# FRAMEWORK REQUIREMENTS:
#   - Symfony: Required for internationalization components
#   - Yii2: Required for i18n module
#   - Laravel: Recommended for multi-language applications
# USE CASES:
#   - Multi-language websites requiring proper locale formatting
#   - E-commerce platforms with international currency/date formatting
#   - Applications handling non-ASCII characters (Chinese, Arabic, etc.)
#   - Proper Unicode string sorting and comparison
#   - IDN domain handling (xn-- Punycode domains)
# MEMORY IMPACT:
#   - ICU library: ~10-15MB resident memory when loaded
#   - Locale data: Additional memory per active locale
# OS COMPATIBILITY:
#   - EL6-10: All supported (libicu-devel available)
# RECOMMENDATION: Enable by default (default 'y'), essential for modern PHP applications
# Related: PHPIMAP, PHPINTLEXTRA, mbstring extension
# Git History: First added 2014-10-21 (bf44db3f), Last modified 2016-02-15 (7240145c)
#PHPINTL='y'  

# PHP Recode Extension - DEPRECATED AND REMOVED
# Character set conversion extension (REMOVED in PHP 7.4+)
# CONFIGURE FLAGS:
#   - --with-recode
# DEPRECATION STATUS:
#   - PHP 5.x-7.3: Deprecated but functional
#   - PHP 7.4+: REMOVED (compilation will FAIL)
#   - Official removal: PHP 7.4.0 (November 2019)
# PHP VERSION COMPATIBILITY:
#   - PHP 5.x: Supported but deprecated
#   - PHP 7.0-7.3: Supported but deprecated
#   - PHP 7.4+: NOT SUPPORTED (extension removed from PHP source)
#   - PHP 8.x: NOT SUPPORTED (extension removed)
# REPLACEMENT EXTENSIONS:
#   - iconv: Built-in, modern character set conversion (preferred)
#   - mbstring: Multi-byte string handling with encoding conversion
# MIGRATION PATH:
#   - recode_string() → iconv() or mb_convert_encoding()
#   - recode_file() → iconv() with file operations
# FUNCTION MIGRATION:
#   - Old: recode_string("utf8..iso88591", $string)
#   - New (iconv): iconv("UTF-8", "ISO-8859-1", $string)
#   - New (mbstring): mb_convert_encoding($string, "ISO-8859-1", "UTF-8")
# WARNING:
#   - DO NOT enable for PHP 7.4+ (compilation failure)
#   - Migrate existing code to iconv or mbstring
#   - No PECL replacement available
# CRITICAL NOTICE:
#   - Extension permanently removed from PHP 7.4+
#   - No future support or security updates
#   - Applications must migrate to iconv/mbstring
# RECOMMENDATION: DO NOT ENABLE - Use iconv (built-in) or mbstring instead
# Related: iconv, mbstring, character encoding
# Git History: First added 2014-04-24 (d962e8b9), Last modified 2016-02-15 (7240145c)
#PHPRECODE=n  # Disable or Enable PHP Recode extension

# PHP SNMP Extension
# SNMP protocol support
# Git History: First added 2014-04-24 (d962e8b9), Last modified 2016-02-15 (7240145c)
#PHPSNMP='y'  

# PHP Microsoft SQL Server Support (PDO ODBC)
# Enable Microsoft SQL Server connectivity via ODBC and FreeTDS protocol
# AUTOMATIC BEHAVIOR:
#   - Smart dependency detection: checks for /usr/include/sql.h (unixODBC)
#   - Smart dependency detection: checks for /usr/include/sqldb.h (FreeTDS)
#   - Auto-installs unixODBC-devel if missing
#   - Auto-installs freetds-devel if missing
#   - Compiles PHP with PDO ODBC support
# CONFIGURE FLAGS:
#   - --with-pdo-odbc=unixODBC,/usr
# PHP EXTENSION PROVIDED:
#   - pdo_odbc: Generic ODBC PDO driver (NOT official Microsoft sqlsrv extension)
# IMPORTANT NOTE:
#   - Uses FreeTDS for TDS protocol (SQL Server communication protocol)
#   - NOT the official Microsoft sqlsrv/pdo_sqlsrv PECL extensions
#   - Generic ODBC driver compatible with any ODBC data source
# DEPENDENCIES:
#   - unixODBC: ODBC driver manager
#   - unixODBC-devel: Development headers for compilation
#   - freetds: Open-source TDS protocol implementation
#   - freetds-devel: Development headers for FreeTDS
# CONFIGURATION FILES:
#   - /etc/odbc.ini: ODBC data source definitions
#   - /etc/odbcinst.ini: ODBC driver registrations
#   - /etc/freetds.conf: FreeTDS server connection settings
# PHP VERSION COMPATIBILITY:
#   - PHP 5.x: Fully compatible
#   - PHP 7.x: Fully compatible
#   - PHP 8.x: Fully compatible
# OS COMPATIBILITY:
#   - EL6-10: All supported (unixODBC and FreeTDS available)
# USE CASES:
#   - Applications connecting to Microsoft SQL Server databases
#   - Legacy SQL Server 2005-2019 connectivity
#   - Azure SQL Database connections
#   - Cross-database applications (MySQL + SQL Server)
# ALTERNATIVE:
#   - Official Microsoft sqlsrv: Install via PECL after PHP compilation
#   - sqlsrv extension: Requires Microsoft ODBC Driver 17+ for SQL Server
# RECOMMENDATION: Enable only if SQL Server connectivity required, consider official sqlsrv for production
# Related: POSTGRESQL, PDO_SQLSRV (PECL), ODBC drivers
# Git History: Added 2016-07-01 (90a78784)
#PHPMSSQL='n'  

# PHP MSSQL Always Reinstall
# Always reinstall mssql extension on PHP recompiles
# Git History: First added 2016-07-05 (4be8b21f), Last modified 2017-09-23 (b6cb97d4)
#PHPMSSQL_ALWAYS='n'  

# PHP Embed SAPI (Server Application Programming Interface)
# Build libphp.so shared library for embedding PHP in C/C++ applications
# CONFIGURE FLAGS:
#   - --enable-embed=shared
# PHP VERSION COMPATIBILITY:
#   - PHP 5.3: NOT supported (hard-disabled, incompatibility)
#   - PHP 5.4+: Fully compatible
#   - PHP 7.x: Fully compatible
#   - PHP 8.x: Fully compatible
# AUTOMATIC BEHAVIOR:
#   - Hard-disabled for PHP 5.3 (compilation incompatibility)
#   - Builds shared library instead of static library (=shared)
# OUTPUT FILES:
#   - Library: /usr/local/lib/libphp.so (~10-20MB shared object)
#   - Headers: /usr/local/include/php/sapi/embed/php_embed.h
# USE CASES:
#   - Nginx Unit: Application server with PHP integration
#   - Embedding PHP in C/C++ applications (custom web servers)
#   - Apache mod_php equivalent for custom servers
#   - CGI alternatives requiring embedded PHP
#   - Custom PHP runtime environments
# INTEGRATION EXAMPLES:
#   - Nginx Unit: Uses libphp.so for PHP application serving
#   - Custom web servers: Embed PHP interpreter directly
#   - Scripting engines: Add PHP as scripting language
# MEMORY IMPACT:
#   - Shared library: ~10-20MB on disk
#   - Runtime: Same as PHP-FPM (no performance difference)
# PERFORMANCE:
#   - No runtime overhead vs PHP-FPM
#   - Faster startup for embedded contexts (library already loaded)
# WARNING:
#   - NOT needed for standard PHP-FPM deployments
#   - Only required if embedding PHP in custom applications
#   - Standard web applications should use PHP-FPM
# RECOMMENDATION: Disable unless using Nginx Unit or embedding PHP (not needed for PHP-FPM)
# Related: PHP-FPM (standard SAPI), Nginx Unit, Apache mod_php
# Git History: Added 2017-09-23 (b6cb97d4)
#PHPEMBED='y'  

# PHP FTP Extension - INSECURE PROTOCOL
# File Transfer Protocol support with plaintext authentication (security risk)
# CONFIGURE FLAGS:
#   - --enable-ftp
# FEATURES PROVIDED:
#   - ftp_connect(): Connect to FTP server (plaintext, port 21)
#   - ftp_ssl_connect(): Connect via FTPS (FTP over SSL/TLS, port 990)
#   - ftp_login(): Authenticate (username/password in plaintext)
#   - ftp_get()/ftp_put(): Download/upload files
#   - ftp_mkdir()/ftp_rmdir(): Directory operations
#   - ftp_pasv(): Passive mode for firewall traversal
# PHP VERSION COMPATIBILITY:
#   - PHP 5.x: Fully compatible
#   - PHP 7.x: Fully compatible
#   - PHP 8.x: Fully compatible
# OS COMPATIBILITY:
#   - EL6-10: All supported
# CRITICAL SECURITY WARNING:
#   - FTP protocol transmits credentials in PLAINTEXT (no encryption)
#   - Username and password visible to network sniffers
#   - Data transfers unencrypted (man-in-the-middle risk)
#   - Legacy protocol from 1971, not secure for modern use
# SECURITY ISSUES:
#   - Authentication: Plaintext username/password transmission
#   - Data transfer: Unencrypted file contents
#   - Session hijacking: Vulnerable to MITM attacks
#   - Firewall traversal: Passive mode security concerns
# SECURE ALTERNATIVES:
#   - SFTP (SSH File Transfer Protocol): Use ssh2 PECL extension
#   - FTPS (FTP over SSL): ftp_ssl_connect() - better but still legacy
#   - HTTPS/WebDAV: Modern encrypted file transfer
#   - S3-compatible storage: AWS S3, Cloudflare R2, Backblaze B2
#   - rsync over SSH: Efficient encrypted synchronization
# MIGRATION RECOMMENDATIONS:
#   - FTP → SFTP (ssh2_connect, ssh2_sftp)
#   - FTP → S3 (AWS SDK for PHP)
#   - FTP → HTTPS (cURL or Guzzle HTTP client)
# USE CASES (LEGACY ONLY):
#   - Legacy systems requiring FTP (migrate ASAP)
#   - Internal networks with FTP-only infrastructure
#   - Temporary file transfers on trusted networks
# MODERN BEST PRACTICE:
#   - DO NOT use FTP for production file transfers
#   - Use SFTP (ssh2) or HTTPS for secure transfers
#   - S3-compatible storage for cloud file management
# FTPS (FTP over SSL) NOTE:
#   - ftp_ssl_connect() provides encryption
#   - Still legacy protocol, SFTP preferred
#   - Better than plain FTP but not modern standard
# RECOMMENDATION: AVOID using FTP - migrate to SFTP, HTTPS, or S3 for security
# Related: ssh2 extension (SFTP), cURL (HTTPS), AWS SDK (S3)
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
#PDOPGSQL_PHPVER='18'

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
# Use REMI repository libzip for ImageMagick 7 + PHP Imagick extension compilation
# REQUIREMENT:
#   - Only applies when IMAGEMAGICK_SEVEN='y'
#   - EL8/9/10 systems compiling ImageMagick 7 from source
# WHY REMI LIBZIP NEEDED:
#   - ImageMagick 7 requires libzip 1.3+ for ZIP delegate
#   - System libzip on EL8 is 1.5.1 (compatible but old)
#   - REMI provides newer libzip with better ImageMagick integration
# AUTOMATIC BEHAVIOR:
#   - Installs libzip-devel from REMI repository
#   - Links ImageMagick 7 against REMI libzip
#   - PHP Imagick extension uses same libzip version
# REMI REPOSITORY:
#   - Auto-enabled via /etc/yum.repos.d/remi.repo
#   - libzip package: remi-libzip-devel
# CONFLICTS AVOIDED:
#   - Prevents libzip version mismatches
#   - Ensures ABI compatibility between ImageMagick and PHP
# USE CASES:
#   - EL8/9 with IMAGEMAGICK_SEVEN='y'
#   - ImageMagick 7 source compilation
#   - PHP Imagick extension against ImageMagick 7
# RECOMMENDATION: Keep enabled when using ImageMagick 7
# Related: IMAGEMAGICK_SEVEN, PHPIMAGICK, PHP_LIBZIP, LIBZIP_VER
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
# Swoole Asynchronous PHP Extension
# High-performance async networking framework with coroutines for PHP
# OS COMPATIBILITY:
#   - EL6: NOT SUPPORTED (incompatibility)
#   - EL7: Fully supported
#   - EL8: Fully supported
#   - EL9: Fully supported
#   - EL10: Fully supported
# PHP VERSION COMPATIBILITY:
#   - PHP 7.0: Supported
#   - PHP 7.1: Supported
#   - PHP 7.2: Supported
#   - PHP 7.3: Supported
#   - PHP 7.4: Supported
#   - PHP 8.0: Supported
#   - PHP 8.1: Supported
#   - PHP 8.2: Supported
#   - PHP 8.3: Supported
#   - PHP 8.4: Supported
# CONFIGURE FLAGS:
#   - --with-swoole
# FEATURES PROVIDED:
#   - Coroutines: Lightweight concurrent execution (async/await style)
#   - Async I/O: Non-blocking network operations
#   - WebSocket: Built-in WebSocket server/client
#   - HTTP/2: Native HTTP/2 server support
#   - TCP/UDP Servers: High-performance network servers
#   - Task Workers: Background task processing
#   - Process Management: Process pool and IPC
# PERFORMANCE IMPACT:
#   - 2-5x faster than PHP-FPM for I/O-bound applications
#   - Lower memory usage per connection vs traditional PHP
#   - Single-process handles thousands of concurrent connections
# USE CASES:
#   - Laravel Octane: Laravel with Swoole acceleration
#   - Hyperf: Microservices framework built on Swoole
#   - Real-time applications: Chat, live updates, WebSocket apps
#   - High-concurrency APIs: Thousands of concurrent requests
#   - Microservices: Service mesh and RPC communication
# FRAMEWORK INTEGRATION:
#   - Laravel Octane: Official Laravel integration
#   - Hyperf: Enterprise microservices framework
#   - Swoole native: Direct Swoole server programming
# CRITICAL WARNING:
#   - EL6 systems: Swoole NOT supported, compilation will fail
#   - Requires EL7 or newer operating system
# MEMORY REQUIREMENTS:
#   - Base: ~10-15MB per worker process
#   - Scales efficiently with concurrent connections
# RECOMMENDATION: Enable for high-performance async applications, Laravel Octane, or microservices (EL7+ only)
# Related: PHP_PARALLEL, PHP_FPM (traditional), NGINX_DEVTOOLSETGCC
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

# PostgreSQL Server and PHP Extension
# Install PostgreSQL database server with PHP pgsql and pdo_pgsql extensions
# AUTOMATIC BEHAVIOR:
#   - Installs PostgreSQL from official PostgreSQL YUM repository
#   - Compiles PHP with PostgreSQL support during menu option 5
#   - Creates pgsql and pdo_pgsql PHP extensions
#   - Version controlled by PDOPGSQL_PHPVER variable (default: auto-detect latest)
# OS COMPATIBILITY:
#   - EL7: PostgreSQL 9.6-15 (15 is max; 16+ not available from PGDG)
#   - EL8: PostgreSQL 14-18 (18 recommended default)
#   - EL9: PostgreSQL 14-18 (18 recommended default)
#   - EL10: PostgreSQL 14-18 (18 recommended default)
# PACKAGES INSTALLED:
#   - postgresql{VER}-server: PostgreSQL database server
#   - postgresql{VER}-devel: Development headers for compilation
#   - postgresql{VER}-libs: Client libraries
#   - postgresql{VER}-contrib: Additional PostgreSQL modules
# CONFIGURE FLAGS:
#   - --with-pgsql=/usr/pgsql-${PDOPGSQL_PHPVER}
#   - --with-pdo-pgsql=/usr/pgsql-${PDOPGSQL_PHPVER}
# PHP EXTENSIONS PROVIDED:
#   - pgsql: Native PostgreSQL extension (pg_connect, pg_query, pg_fetch_*)
#   - pdo_pgsql: PDO driver for PostgreSQL (PDO::PGSQL)
# PHP VERSION COMPATIBILITY:
#   - PHP 5.x: Fully compatible
#   - PHP 7.x: Fully compatible
#   - PHP 8.x: Fully compatible
# DEPENDENCIES:
#   - PostgreSQL official YUM repository
#   - /usr/pgsql-{VER}/ installation path
# USE CASES:
#   - Applications requiring PostgreSQL instead of MySQL/MariaDB
#   - Multi-database environments (PostgreSQL + MariaDB)
#   - Laravel, Symfony, Django applications with PostgreSQL backend
#   - Data warehousing and analytics platforms
# POST-INSTALLATION:
#   - Server binary: /usr/pgsql-{VER}/bin/postgres
#   - Client tools: psql, pg_dump, pg_restore
#   - Service: postgresql-{VER}.service
#   - Configuration: /var/lib/pgsql/{VER}/data/postgresql.conf
# RECOMMENDATION: Enable only if application requires PostgreSQL database
# Related: PHPMSSQL, SQLITEDB, MYSQLJEMALLOC
# Git History: First added 2015-09-19 (fac86b32), Last modified 2016-07-01 (90a78784)
#POSTGRESQL='n'

# PostgreSQL Branch Version
# PostgreSQL major version to install
# Reference: https://www.postgresql.org/
# Git History: Added 2019-02-07 (57108912)
#POSTGRESQL_BRANCHVER='18'

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
# Let's Encrypt SSL Certificate Auto-Detection and Integration
# Enables automatic Let's Encrypt certificate detection and ACME client integration during vhost creation.
# AUTOMATIC BEHAVIOR:
#   - When LETSENCRYPT_DETECT='y': Automatically detects acmetool.sh addon during vhost creation (nginx_addvhost.inc:694)
#   - Displays Let's Encrypt certificate issuance options during nv (nginx vhost) wizard
#   - Enables 'le', 'led', 'lelive', 'lelived' SSL certificate options (nginx_addvhost.inc:1520,1600)
#   - Automatically creates .well-known/acme-challenge/ directory for ACME validation
#   - Integrates with acmetool.sh, acme.sh, or certbot for certificate management
# NGINX VERSION REQUIREMENTS:
#   - Works with all nginx versions (Let's Encrypt uses HTTP-01, DNS-01, TLS-ALPN-01 challenges)
#   - Nginx 1.19.0+ recommended for modern SSL configuration
# UPGRADE BEHAVIORS:
#   - No direct upgrade logic in nginx_upgrade.inc
#   - Preserves existing Let's Encrypt certificate paths during nginx upgrades
# PATCH BEHAVIORS:
#   - No specific patches in nginx_patch.inc (ACME challenge standard protocol)
# VHOST BEHAVIORS: (from nginx_addvhost.inc)
#   - Line 362-363: if LETSENCRYPT_DETECT != 'y' displays informational message about enabling feature
#   - Line 369,371: Instructions for enabling LETSENCRYPT_DETECT='y' in /etc/centminmod/custom_config.inc
#   - Line 694: if acmetool.sh exists and LETSENCRYPT_DETECT='y' shows Let's Encrypt options
#   - Line 1520: if LETSENCRYPT_DETECT='y' enables SSL certificate issuance workflow
#   - Line 1600: if LETSENCRYPT_DETECT='y' and vhostssl='le' triggers certificate generation
#   - Creates nginx location block: location ~ /.well-known/acme-challenge/ { root /usr/local/nginx/html; }
# CONFIGURE FLAGS:
#   - No nginx ./configure flags affected (ACME challenge uses HTTP verification)
#   - Requires --with-http_ssl_module for SSL certificate usage
# OS COMPATIBILITY:
#   - EL6/7/8/9/10: Full compatibility (ACME protocol HTTP/DNS challenges)
#   - Requires: curl, openssl for ACME client communication
#   - Works with acmetool, acme.sh, certbot, lego ACME clients
# PERFORMANCE IMPACT:
#   - No runtime performance impact (certificates generated/renewed offline)
#   - Certificate renewals: Automatic every 60 days (cron job)
#   - .well-known/acme-challenge/ location adds minimal overhead (rarely accessed)
# SECURITY IMPLICATIONS:
#   - Let's Encrypt provides free DV (Domain Validated) certificates
#   - 90-day certificate lifetime forces regular renewal (security best practice)
#   - ACME protocol secure: HTTP-01 challenge validates domain ownership
#   - Recommended: Use DNS-01 challenge for wildcard certificates
# CONFLICTS:
#   - No conflicts with other nginx modules
#   - Compatible with NGINX_VHOSTSSL='y', VHOST_CFAUTHORIGINPULL='y'
#   - Cannot use with self-signed certificates (choose one or the other)
# RECOMMENDATION:
#   - Enable for production sites needing free SSL certificates
#   - Recommended: All public-facing websites should use SSL
#   - Disable if: Using commercial SSL certificates, internal-only sites
#   - Pair with NGINX_VHOSTSSL='y' for SSL vhost creation
#   - Setup cron job: Automatic certificate renewal every 60 days
# Related: NGINX_VHOSTSSL, VHOST_CFAUTHORIGINPULL, acmetool.sh addon
# Reference: https://letsencrypt.org/, https://github.com/acmesh-official/acme.sh
# Git History: Added with Let's Encrypt integration ~2015-2016
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

# Parallel Gzip Version (pigz)
# Multi-threaded gzip compression tool version
# USAGE: Menu option 17 (Multi-thread compression), backup scripts, log compression
# PERFORMANCE:
#   - Parallel threads based on CPU core detection
#   - ~4-6x faster compression on multi-core systems than single-threaded gzip
# Git History: Added ~2015-03-10
#PIGZ_VER='2.7'

# Zstandard Compression Version (zstd)
# High-ratio, high-speed modern compression algorithm version
# USAGE: Menu option 17 (Multi-thread compression), modern backup systems
# PERFORMANCE:
#   - Better compression ratio than gzip
#   - Faster decompression than bzip2
#   - Multi-threaded operation support
# Git History: Added ~2017-08-15
#ZSTD_VER='1.5.6'

# Parallel Bzip2 Version (pbzip2)
# Multi-threaded bzip2 compression tool version
# USAGE: Menu option 17 (Multi-thread compression), tar.bz2 archives
# PERFORMANCE:
#   - Parallel processing on multi-core systems
#   - Better compression ratio than gzip
# Git History: Added ~2015-03-10
#PBZIP_VER='1.1.13'

# Multi-threaded Bzip2 Version (lbzip2)
# Alternative multi-threaded bzip2 implementation version
# USAGE: Menu option 17 (Multi-thread compression), alternative to pbzip2
# PERFORMANCE:
#   - Different approach to parallel bzip2 compression
#   - May perform better than pbzip2 on certain hardware
# Git History: Added ~2015-03-10
#LBZIP_VER='2.5'

# LZ4 Compression Version
# Ultra-fast compression algorithm version
# USAGE: Menu option 17 (Multi-thread compression), real-time compression scenarios
# PERFORMANCE:
#   - Fastest compression speed of all compression tools
#   - Moderate compression ratio
#   - Ideal for speed-critical applications
# Git History: Added ~2018-06-10
#LZFOUR_VER='1.10.0'

# Lzip Compression Version
# LZMA-based compression with data recovery capabilities version
# USAGE: Menu option 17 (Multi-thread compression), long-term archival
# PERFORMANCE:
#   - Excellent compression ratio
#   - Built-in error detection and recovery
#   - Good for long-term storage
# Git History: Added ~2019-02-12
#LZIP_VER='1.21'

# Parallel Lzip Version (plzip)
# Parallel implementation of lzip compression version
# USAGE: Menu option 17 (Multi-thread compression), parallel LZMA compression
# PERFORMANCE:
#   - Multi-threaded LZMA compression
#   - Maintains excellent compression ratio with parallel processing
# Git History: Added ~2019-06-01
#PLZIP_VER='1.8'

################################################################################
# 49. PERFORMANCE TOOLS
################################################################################

# Microsoft mimalloc Memory Allocator for Nginx
# Replace glibc malloc with Microsoft's mimalloc for improved memory performance and reduced fragmentation.
# AUTOMATIC BEHAVIOR:
#   - Auto-installs mimalloc RPM from Centmin Mod repo (line 3158-3171)
#   - Auto-disables GPERFTOOLS_SOURCEINSTALL when enabled (line 3173-3179)
#   - Auto-disables JEMALLOC when enabled (line 3177-3178, 3187)
#   - RPM installation by OS: EL7/8/9/10 versions (line 3162-3170)
# NGINX VERSION COMPATIBILITY:
#   - All Nginx versions compatible
#   - Works with all modules and configurations
# OS COMPATIBILITY:
#   - EL7: mimalloc-custom-${VERSION}-1.el7.x86_64.rpm (line 3162-3164)
#   - EL8: mimalloc-custom-${VERSION}-1.el8.x86_64.rpm (line 3159-3161)
#   - EL9: mimalloc-custom-${VERSION}-1.el9.x86_64.rpm (line 3165-3167)
#   - EL10: mimalloc-custom-${VERSION}-1.el10.x86_64.rpm (line 3168-3170)
# CONFIGURE FLAGS:
#   - --with-ld-opt: -lmimalloc (line 3177)
#   - Uses MIMALLOC_LD variable
#   - Links against /usr/local/nginx-dep/lib/libmimalloc.so
# LIBRARY INSTALLATION:
#   - Library: /usr/local/nginx-dep/lib/libmimalloc.so
#   - Headers: /usr/local/nginx-dep/include/mimalloc.h
#   - Installed via RPM from LOCALCENTMINMOD_MIRROR
# CONFLICTS:
#   - Mutually exclusive with GPERFTOOLS_SOURCEINSTALL='y' (tcmalloc)
#   - Disables jemalloc when enabled (JEMALLOC_LD='')
# PERFORMANCE IMPACT:
#   - Memory allocation: 10-25% faster than glibc malloc
#   - Memory usage: 5-15% lower (reduced fragmentation)
#   - Throughput: 5-10% higher on memory-intensive workloads
#   - Best for: High-concurrency Nginx with many connections
# MIMALLOC FEATURES:
#   - Secure: Guard pages, free-list randomization
#   - Low fragmentation: Superior to glibc malloc
#   - Thread-local caching: Reduced lock contention
#   - Transparent huge pages support
# PCRE2 COMPATIBILITY:
#   - NGINX_MIMALLOC displays with NGINX_PCRE_TWO checks (line 782, 802)
#   - Fully compatible with PCRE2 dynamic linking
# USE CASES:
#   - High-traffic production Nginx (100K+ requests/sec)
#   - Memory-intensive workloads (large buffers, caching)
#   - Long-running Nginx processes (reduced fragmentation over time)
#   - Modern CPUs (Intel Xeon, AMD EPYC) - benefits from thread-local caching
# MEMORY ALLOCATOR COMPARISON:
#   - mimalloc: Best overall (performance + security + low fragmentation)
#   - jemalloc: Similar performance, more mature
#   - tcmalloc: Profiling tools, slightly slower
#   - glibc malloc: Slowest, highest fragmentation
# RECOMMENDATION: Enable for production Nginx deployments handling high concurrency.
#   Mimalloc provides best-in-class memory performance with security features. Preferred
#   over jemalloc for modern deployments (2022+).
# Related: GPERFTOOLS_SOURCEINSTALL (tcmalloc), jemalloc (auto-installed separately)
# Reference: https://github.com/microsoft/mimalloc
# Git History: Analysis added 2025-10-25, original Git history not available
#NGINX_MIMALLOC='n'

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
