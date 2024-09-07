[![GitHub stars](https://img.shields.io/github/stars/centminmod/centminmod.svg?style=flat-square)](https://github.com/centminmod/centminmod/stargazers) [![GitHub forks](https://img.shields.io/github/forks/centminmod/centminmod.svg?style=flat-square)](https://github.com/centminmod/centminmod/network) [![GitHub issues](https://img.shields.io/github/issues/centminmod/centminmod.svg?style=flat-square)](https://github.com/centminmod/centminmod/issues) [![GitHub license](https://img.shields.io/badge/license-GPL-blue.svg?style=flat-square)](https://raw.githubusercontent.com/centminmod/centminmod/master/license.txt) [![AlmaLinux 8](https://github.com/centminmod/centminmod-workflows/actions/workflows/main.yml/badge.svg?branch=master)](https://github.com/centminmod/centminmod-workflows/actions/workflows/main.yml) [![AlmaLinux 9](https://github.com/centminmod/centminmod-workflows/actions/workflows/almalinux9.yml/badge.svg)](https://github.com/centminmod/centminmod-workflows/actions/workflows/almalinux9.yml) [![Rocky Linux 8](https://github.com/centminmod/centminmod-workflows/actions/workflows/rockylinux8.yml/badge.svg)](https://github.com/centminmod/centminmod-workflows/actions/workflows/rockylinux8.yml) [![Rocky Linux 9](https://github.com/centminmod/centminmod-workflows/actions/workflows/rockylinux9.yml/badge.svg)](https://github.com/centminmod/centminmod-workflows/actions/workflows/rockylinux9.yml)

![Centmin Mod](/centmin-mod-logo2.jpg)

Centmin Mod LEMP stack was created and is developed by George Liu and can be installed via Unattended Command Line method below or via latest install instructions on [Official Install Guide](https://centminmod.com/install.html):

1. Centmin Mod Unattended Command Line Install (highly recommended)
2. Centmin Mod installed via Git (deprecated no longer supported)

After install bookmark and read the [Getting Started Guide](https://centminmod.com/getstarted.html) and check out the Centmin Mod Community forum at [https://community.centminmod.com](https://community.centminmod.com)

## Centmin Mod Unattended Command Line Install

Fastest method of install and allows fully unattended installation. Just type this command as root user in SSH on a fresh CentOS 7 server. Installation should take between 15-30 minutes on a fast server or up to 50-70 minutes on a slower server depending on server specs and your server's network connectivity and download speed.

As at July 1, 2024, Centmin Mod versions are undergoing a transition version branch wise:

1. Previous 124.00stable is now moving to 131.00stable. 131.00stable is essentially based on the well tested 130.00beta01 branch. All development and changes made in 130.00beta01 are now in 131.00stable.
2. A new 140.00beta01 branch has started and this is also based off of 130.00ebat01 branch but with additional development and code to eventually support EL8+ operating systems like CentOS 8, Alma Linux 8 and Rocky Linux 8. This branch will take over from 130.00beta01 as the development beta branch. EL8+ operating system support is now officially supported.

### For latest 131.00stable install

Centmin Mod installers for fresh AlmaLinux or Rocky Linux operating system based servers with minimum 4GB installed memory requirements.

PHP 8.3.x default stable installer.

```
yum -y update
curl -O https://centminmod.com/installer83.sh && chmod 0700 installer83.sh && bash installer83.sh
```

PHP 8.2.x default stable installer.

```
yum -y update
curl -O https://centminmod.com/installer82.sh && chmod 0700 installer82.sh && bash installer82.sh
```

PHP 8.1.x default stable installer.

```
yum -y update
curl -O https://centminmod.com/installer81.sh && chmod 0700 installer81.sh && bash installer81.sh
```

PHP 8.0.x default stable installer with backported security fixes.

```
yum -y update
curl -O https://centminmod.com/installer80.sh && chmod 0700 installer80.sh && bash installer80.sh
```

PHP 7.4.x default stable installer with backported security fixes.

```
yum -y update
curl -O https://centminmod.com/installer74.sh && chmod 0700 installer74.sh && bash installer74.sh
```

### For latest 140.00beta01 install

Centmin Mod installers for fresh AlmaLinux or Rocky Linux operating system based servers with minimum 4GB installed memory requirements.

PHP 8.3.x default beta installer.

```
yum -y update
curl -O https://centminmod.com/betainstaller83.sh && chmod 0700 betainstaller83.sh && bash betainstaller83.sh
```

PHP 8.2.x default beta installer.

```
yum -y update
curl -O https://centminmod.com/betainstaller82.sh && chmod 0700 betainstaller82.sh && bash betainstaller82.sh
```

PHP 8.1.x default beta installer.

```
yum -y update
curl -O https://centminmod.com/betainstaller81.sh && chmod 0700 betainstaller81.sh && bash betainstaller81.sh
```

PHP 8.0.x default beta installer with backported security fixes.

```
yum -y update
curl -O https://centminmod.com/betainstaller80.sh && chmod 0700 betainstaller80.sh && bash betainstaller80.sh
```

PHP 7.4.x default beta installer with backported security fixes.

```
yum -y update
curl -O https://centminmod.com/betainstaller74.sh && chmod 0700 betainstaller74.sh && bash betainstaller74.sh
```

You can also customise your installs via pre-populating the persistent config file, `/etc/centminmod/custom_config.inc` with overriding variables instead of directly editing `centmin.sh` file **BEFORE** running the the `betainstaller.sh`. See examples discussed on the forums [here](https://community.centminmod.com/threads/discussion-how-do-you-initially-install-setup-your-centmin-mod-server.14736/).

## Contributing

Below are guidelines for contributing code wise. 

* [Centmin Mod Insights forum](https://community.centminmod.com/forums/centmin-mod-insights.20/) is the place to ask questions or clarifications about how Centmin Mod works under the hood.
* Every Git committed code also has a corresponding forum thread in [Centmin Mod Github.com Repo forums](https://community.centminmod.com/link-forums/centmin-mod-github-com-repository.13/) if you're more comfortable using the forums instead of the [Github issue tracker](https://github.com/centminmod/centminmod/issues).

# Bug Reports

* Bug reports can be made via [Github issue tracker](https://github.com/centminmod/centminmod/issues) or Centmin Mod official forum's [Bug Reports forums](https://community.centminmod.com/forums/bug-reports.12/).

# Pull Requests

* Pull requests can be done against the current [Github active branches](https://github.com/centminmod/centminmod/branches/active) - currently being [131.00stable](https://github.com/centminmod/centminmod/tree/131.00stable) and [140.00beta01](https://github.com/centminmod/centminmod/tree/140.00beta01). Usually once weekly, active branch changes are then merged into [master branch](https://github.com/centminmod/centminmod).

# Suggestions

* Suggestions and feedback can be made via [Github issue tracker](https://github.com/centminmod/centminmod/issues) or Centmin Mod official forum's [Feature Requests & Suggestions forums](https://community.centminmod.com/forums/feature-requests-suggestions.11/).