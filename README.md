Note: 1.2.3-eva2000.08 stable = Github branch 123.08stable and is not official released until after July 31, 2015

Centmin Mod can be installed via 3 different ways:

1. Centmin Mod Unattended Command Line Install
2. Centmin Mod installed via Git
3. Centmin Mod installed via Github zip download

After install bookmark and read the [Getting Started Guide](http://centminmod.com/getstarted.html) and check out the Centmin Mod Community forum at [https://community.centminmod.com](https://community.centminmod.com)

## Centmin Mod Unattended Command Line Install

Fastest method of install and allows fully unattended installation. Just type this command as root user in SSH on a fresh CentOS 6 or CentOS 7 server. Installation should take between 15-30 minutes on a fast server or up to 50-70 minutes on a slower server depending on server specs and your server's network connectivity and download speed.

### For latest 1.2.3-eva2000.08 stable install

    curl -O https://centminmod.com/installer.sh && chmod 0700 installer.sh && bash installer.sh

### For latest 1.2.3-eva2000.09 beta install

    curl -O https://centminmod.com/betainstaller.sh && chmod 0700 betainstaller.sh && bash betainstaller.sh

## Centmin Mod installed via Git    

Type as root user in SSH these commands, Centmin Mod will have it's install setup at /usr/local/src/centminmod. Replace `branchname=123.08stable` with `branchname=123.09beta01` if you want to install the beta version.

    yum -y install git wget nano bc unzip
    cd /usr/local/src
    branchname=123.08stable
    git clone -b ${branchname} --depth=1 https://github.com/centminmod/centminmod.git centminmod
    cd centminmod

Then to install either type

for menu mode run centmin.sh and select menu option 1 to install

    ./centmin.sh

or for CLI install mode

    ./centmin.sh install    

## Centmin Mod installed via Github zip download


### Step 1.


Select the branch you want to install from list at https://github.com/centminmod/centminmod/branches and use the following command (in your SSH console) to set it:

    branchname=123.08stable

### Step 2.


To get the Centmin Mod files, run the following commands in a console as root. These commands will download the files to `/usr/local/src/centminmod`

    yum -y install wget nano bc unzip
    branchname=123.08stable
    wget -O /usr/local/src/${branchname}.zip https://github.com/centminmod/centminmod/archive/${branchname}.zip
    cd /usr/local/src
    unzip ${branchname}.zip
    mv centminmod-${branchname} centminmod
    cd centminmod

To run the actual install in menu mode run `centmin.sh` (still as root) and select menu option 1 to install:

    ./centmin.sh

or to install in CLI mode:

    ./centmin.sh install