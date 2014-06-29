##################################
Centmin Mod Menu based Nginx Auto Installer 
##################################
* Getting Started Guide - http://centminmod.com/getstarted.html
* Latest Centmin Mod version - http://centminmod.com       
* Centmin Mod FAQ - http://centminmod.com/faq.html
* Change Log - http://centminmod.com/changelog.html
* Google+ Page latest news http://centminmod.com/gpage
* Community Forums https://community.centminmod.com (signup)

##################################
This script is NOT SUPPORTED in any way, shape or form. You may get help from fellow Centmin Mod users on Centmin Mod Google+ Community site at http://centminmod.com/gcom.

However, if you have noticed a bug please feel free to let me know at http://centminmod.com/gcom and I will fix it as soon as possible.

##################################
License:

This script is licensed under GPLv3 (or higher - at your discretion, if available). For details please read the included license.txt or visit http://www.gnu.org/licenses/gpl.html

But basically, feel free to modify or use this script as you see fit. If you have made modifications
that you feel would be useful to be included in this, let me know where I can download a copy and I
will consider adding them.

##################################
Installation/usage:

Follow instructions on official Centmin Mod FAQ Page at http://centminmod.com/download.html

##################################
Configuration:

If you are using Xen Paravirtualization and are running a 32bit OS on a 64bit host node, then you must
uncomment the "#ARCH_OVERRIDE='i386'" line (line 25) of the script, else it will not function correctly.
(to uncomment, simply remove the hash - #).

Normally there is only one thing you will need to change yourself, and that will be the timezone.
You would then change the "ZONEINFO" line at top of centmin.sh script. So using Los Angeles as the example the line would be changed to: ZONEINFO=America/Los_Angeles. For full details read http://centminmod.com/datetimezones.html

##################################
Post-installation:

I have included configuration files for MySQL, Nginx, NSD and PHP-FPM which should generally be fine for use in any environment (as in, both production and testing). However, feel free to edit these if you wish.

By default the script will start any services it installs, including Nginx. So if you browse to your server IP
after installation, you should see our Nginx test page, obviously feel free to replace this.

##################################
Vhost and NSD DNS:

Full instructions on official Centmin Mod FAQ Page at http://centminmod.com/nginx_domain_dns_setup.html

If you cd to /home/nginx/domains/demo.com you will see that I have setup an example folder structure for you.

Any public content (e.g. html, php etc. files) should go in the public folder, logs will be placed in the logs
folder by Nginx, any private files (e.g. php configuration files etc.) should be put in the private folder
(as it is not accessible by the Ibserver), backups obviously go in the backups folder.

To get a live domain up and running, simply rename the demo.com folder to whatever your domain is called, then edit /usr/local/nginx/conf/conf.d/virtual.conf to reflect these changes - for most setups a simple find/replace with your domain name (without the www.) should be fine. Or follow outline at http://centminmod.com/nginx_domain_dns_setup.html

Also, if you are using NSD an example zone for "demo.com" is included, a find/replace for demo.com with your domain (without www.), and 192.192.192.192 with your server IP address in  /etc/nsd/master/demo.com.zone and replace demo.com with your domain (without www.) in /etc/nsd/nsd.conf should be enough to get you up and running.

##################################
Centmin Mod Official Addons:

Centmin Mod will be releasing official addons which are standalone scripts which extend functionality. You will find them listed at http://centminmod.com/addons.html

--------------------------
Please bookmark:
--------------------------
* Getting Started Guide - http://centminmod.com/getstarted.html
* Latest Centmin Mod version - http://centminmod.com       
* Centmin Mod FAQ - http://centminmod.com/faq.html
* Change Log - http://centminmod.com/changelog.html
* Google+ Page latest news http://centminmod.com/gpage
* Google+ Community Forum http://centminmod.com/gcom