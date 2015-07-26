Note: 1.2.3-eva2000.08 stable = Github branch 123.08stable and is not official released until after July 31, 2015

Centmin Mod can be installed via 3 different ways:

1. Centmin Mod Unattended Command Line Install
2. Centmin Mod installed via Git
3. Centmin Mod installed via Github zip download

After install bookmark and read the [Getting Started Guide](http://centminmod.com/getstarted.html) and check out the Centmin Mod Community forum at [https://community.centminmod.com](https://community.centminmod.com)

## Centmin Mod Unattended Command Line Install

Fastest method of install and allows fully unattended installation. Just type this command as root user in SSH on a fresh CentOS 6 or CentOS 7 server. Installation should take between 15-30 minutes on a fast server or up to 50-70 minutes on a slower server depending on server specs and your server's network connectivity and download speed.

### For latest 1.2.3-eva2000.08 stable install


    curl -sL http://centminmod.com/stableinstaller.sh | bash

or

    curl -sL http://centminmod.com/installer.sh | bash

### For latest 1.2.3-eva2000.08 beta install


    curl -sL http://centminmod.com/betainstaller.sh | bash

## Centmin Mod installed via Git    

Type as root user in SSH these commands, Centmin Mod will have it's install setup at /usr/local/src/centminmod

    yum -y install git wget nano bc unzip
    cd /usr/local/src
    git clone https://github.com/centminmod/centminmod.git centminmod
    cd centminmod

Then to install either type

for menu mode

    ./centmin.sh

or for CLI install mode

    ./centmin.sh install    

## Centmin Mod installed via Github zip download


### Step 1.


Select the branch you want to install from list at https://github.com/centminmod/centminmod/branches and define it in the variable named branchname typed on SSH command line as follows.

    branchname=123.08stable

### Step 2.


Actual install, type as root user in SSH these commands, Centmin Mod will have it's install setup at /usr/local/src/centminmod

    yum -y install wget nano bc unzip
    branchname=123.08stable
    wget -O /usr/local/src/${branchname}.zip https://github.com/centminmod/centminmod/archive/${branchname}.zip
    cd /usr/local/src
    unzip ${branchname}.zip
    mv centminmod-${branchname} centminmod
    cd centminmod

Then to install either type

for menu mode

    ./centmin.sh

or for CLI install mode

    ./centmin.sh install