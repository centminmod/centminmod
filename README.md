[![GitHub stars](https://img.shields.io/github/stars/centminmod/centminmod.svg?style=flat-square)](https://github.com/centminmod/centminmod/stargazers) [![GitHub forks](https://img.shields.io/github/forks/centminmod/centminmod.svg?style=flat-square)](https://github.com/centminmod/centminmod/network) [![GitHub issues](https://img.shields.io/github/issues/centminmod/centminmod.svg?style=flat-square)](https://github.com/centminmod/centminmod/issues) [![GitHub license](https://img.shields.io/badge/license-GPL-blue.svg?style=flat-square)](https://raw.githubusercontent.com/centminmod/centminmod/master/license.txt) [![AlmaLinux 8](https://github.com/centminmod/centminmod-workflows/actions/workflows/main.yml/badge.svg?branch=master)](https://github.com/centminmod/centminmod-workflows/actions/workflows/main.yml) [![AlmaLinux 9](https://github.com/centminmod/centminmod-workflows/actions/workflows/almalinux9.yml/badge.svg)](https://github.com/centminmod/centminmod-workflows/actions/workflows/almalinux9.yml)

![Centmin Mod](/centmin-mod-logo2.jpg)

Centmin Mod can be installed via Unattended Command Line method below or via latest install instructions on [Official Install Guide](https://centminmod.com/install.html):

1. Centmin Mod Unattended Command Line Install (highly recommended)
2. Centmin Mod installed via Git (deprecated no longer supported)

After install bookmark and read the [Getting Started Guide](https://centminmod.com/getstarted.html) and check out the Centmin Mod Community forum at [https://community.centminmod.com](https://community.centminmod.com)

## Centmin Mod Unattended Command Line Install

Fastest method of install and allows fully unattended installation. Just type this command as root user in SSH on a fresh CentOS 7 server. Installation should take between 15-30 minutes on a fast server or up to 50-70 minutes on a slower server depending on server specs and your server's network connectivity and download speed.

As at May 8th, 2022, Centmin Mod versions are undergoing a transition version branch wise:

1. Previous 123.08stable is now moving to 124.00stable. 124.00stable is essentially based on the well tested 123.09beta01 branch. All development and changes made in 123.09beta01 are now in 124.00stable.
2. Previous 123.09beta01 code is now in 124.00stable
3. A new 130.00beta01 branch has started and this is also based off of 123.09ebat01 branch but with additional development and code to eventually support EL8+ operating systems like CentOS 8, Alma Linux 8 and Rocky Linux 8. This branch will take over from 123.09beta01 as the development beta branch. EL8+ operating system support isn't fully complete as at May 8th, 2022. So installation on CentOS 7.x is recommended.

### For latest 124.00stable install

default PHP 7.4.x installation

    yum -y update; curl -O https://centminmod.com/installer.sh && chmod 0700 installer.sh && bash installer.sh

### For latest 130.00beta01 install

PHP 7.4.x default beta installer.

    yum -y update; curl -O https://centminmod.com/betainstaller.sh && chmod 0700 betainstaller.sh && bash betainstaller.sh

PHP 8.2.x default beta installer.

    yum -y update; curl -O https://centminmod.com/betainstaller82.sh && chmod 0700 betainstaller82.sh && bash betainstaller82.sh

PHP 8.1.x default beta installer.

    yum -y update; curl -O https://centminmod.com/betainstaller81.sh && chmod 0700 betainstaller81.sh && bash betainstaller81.sh

PHP 8.0.x default beta installer.

    yum -y update; curl -O https://centminmod.com/betainstaller80.sh && chmod 0700 betainstaller80.sh && bash betainstaller80.sh

PHP 7.4.x default beta installer.

    yum -y update; curl -O https://centminmod.com/betainstaller74.sh && chmod 0700 betainstaller74.sh && bash betainstaller74.sh

PHP 7.3.x default beta installer. See [PHP 7.3 release information](https://community.centminmod.com/threads/php-7-3-0-7-2-13-7-1-25-7-0-33-5-6-39-released.16184/) and [PHP 7.3 vs 7.2 vs 7.1 vs 7.0 benchmarks](https://community.centminmod.com/threads/php-7-3-vs-7-2-vs-7-1-vs-7-0-php-fpm-benchmarks.16090/).

    yum -y update; curl -O https://centminmod.com/betainstaller73.sh && chmod 0700 betainstaller73.sh && bash betainstaller73.sh

PHP 7.2.x default beta installer

    yum -y update; curl -O https://centminmod.com/betainstaller72.sh && chmod 0700 betainstaller72.sh && bash betainstaller72.sh

PHP 7.1.x default beta installer

    yum -y update; curl -O https://centminmod.com/betainstaller71.sh && chmod 0700 betainstaller71.sh && bash betainstaller71.sh

PHP 7.0.x default beta installer

    yum -y update; curl -O https://centminmod.com/betainstaller7.sh && chmod 0700 betainstaller7.sh && bash betainstaller7.sh

default PHP 5.6 beta installer

    yum -y update; curl -O https://centminmod.com/betainstaller56.sh && chmod 0700 betainstaller56.sh && bash betainstaller56.sh

You can also customise your installs via pre-populating the persistent config file, `/etc/centminmod/custom_config.inc` with overriding variables instead of directly editing `centmin.sh` file **BEFORE** running the the `betainstaller.sh`. See examples discussed on the forums [here](https://community.centminmod.com/threads/discussion-how-do-you-initially-install-setup-your-centmin-mod-server.14736/).

## Contributing

Below are guidelines for contributing code wise. 

* [Centmin Mod Insights forum](https://community.centminmod.com/forums/centmin-mod-insights.20/) is the place to ask questions or clarifications about how Centmin Mod works under the hood.
* Every Git committed code also has a corresponding forum thread in [Centmin Mod Github.com Repo forums](https://community.centminmod.com/link-forums/centmin-mod-github-com-repository.13/) if you're more comfortable using the forums instead of the [Github issue tracker](https://github.com/centminmod/centminmod/issues).

# Bug Reports

* Bug reports can be made via [Github issue tracker](https://github.com/centminmod/centminmod/issues) or Centmin Mod official forum's [Bug Reports forums](https://community.centminmod.com/forums/bug-reports.12/).

# Pull Requests

* Pull requests can be done against the current [Github active branches](https://github.com/centminmod/centminmod/branches/active) - currently being [124.00stable](https://github.com/centminmod/centminmod/tree/124.00stable) and [130.00beta01](https://github.com/centminmod/centminmod/tree/130.00beta01). Usually once weekly, active branch changes are then merged into [master branch](https://github.com/centminmod/centminmod).

# Suggestions

* Suggestions and feedback can be made via [Github issue tracker](https://github.com/centminmod/centminmod/issues) or Centmin Mod official forum's [Feature Requests & Suggestions forums](https://community.centminmod.com/forums/feature-requests-suggestions.11/).